package communication;

import model.Data;
import model.PacketType;
import model.SensorData;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

import model.StateClifton;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import static communication.Config.MAGIC_HEADER;
import static communication.Config.MAGIC_LENGTH;

/**
 * Runnable receiver that reads sensor data packets from an input stream,
 * detects synchronization headers, and dispatches received data to the processing Communication layer.
 */
public class XBeeReceiver implements Runnable {

    private static final Logger log = LogManager.getLogger(XBeeReceiver.class);
    private final InputStream input;
    private final Communication communication;
    // used for testing and simulation
    private final boolean skipFindSync;
    private volatile boolean isRunning;

    public XBeeReceiver(InputStream input, Communication communication, boolean skipFindSync) {
        this.input = input;
        this.communication = communication;
        this.skipFindSync = skipFindSync;
    }

    /**
     * Scans the given input stream for a predefined synchronization sequence (MAGIC_HEADER).
     *
     * @param input the InputStream to read from
     * @return true if the sync header is found, false if the stream ends
     * @throws IOException if an I/O error occurs while reading the stream
     */
    private boolean findSync(InputStream input) throws IOException {
        byte[] buffer = new byte[MAGIC_LENGTH];
        int i, bytesRead = 0;
        byte b;

        while (bytesRead < MAGIC_LENGTH) {
            b = (byte) input.read();
            if (b == -1) return false; // Stream ended
            buffer[bytesRead++] = b;
        }

        while (true) {
            if (Arrays.equals(buffer, MAGIC_HEADER)) {
                return true;
            }
            for (i = 0; i < MAGIC_LENGTH - 1; i++) {
                buffer[i] = buffer[i + 1];
            }
            b = (byte) input.read();
            if (b == -1) return false;
            buffer[MAGIC_LENGTH - 1] = b;
        }
    }

    @Override
    public void run() {
        isRunning = true;
        while (isRunning) {
            try {
                processNextMessage();
            } catch (IOException e) {
                log.error("IOException: {}", e.getMessage());
                throw new RuntimeException(e);
            }
        }
    }

    /**
     * Processes the next incoming message from the input stream.
     *
     * If synchronization is required, attempts to locate the sync header before proceeding.
     * Afterward parses the received data and notifies the communication layer.
     *
     * @throws IOException if an I/O error occurs while reading the input stream
     */
    private void processNextMessage() throws IOException {
        if (!skipFindSync && !findSync(input)) return;

        int typeId = input.read();
        int seq = input.read();

        if (typeId == -1 || seq == -1) return;

        log.trace("Received header: typeId({}), seq({})", typeId, seq);

        PacketType type = PacketType.fromId(typeId);
        if (type == PacketType.SENSORS) {
            Data data = handleSensors(input, type);
            communication.onPacketReceived(seq, data);
        } else {
            log.warn("Unsupported packet type: {}", typeId);
        }
    }

    public void stop() {
        this.isRunning = false;
        try {
            input.close();
        } catch (IOException e) {
            log.warn("Could not close input stream: {}", e.getMessage());
        }
    }

    private Data handleSensors(InputStream inputStream, PacketType packet) throws IOException {
        byte[] raw = inputStream.readNBytes(packet.getPayloadSize());
        if (raw.length != packet.getPayloadSize()) throw new IOException("Incomplete packet received");
        ByteBuffer buffer = ByteBuffer.wrap(raw).order(ByteOrder.LITTLE_ENDIAN);
        byte statusFields = buffer.get();

        // TODO: parse ack bit status
        // short ackTo =  ... ;
        boolean error = (statusFields & 0b10000000) != 0;
        StateClifton stateClifton = StateClifton.fromStatusByte(statusFields);
        byte waypoint = (byte) (statusFields & 0b00011111);
        short windDirection = buffer.getShort();
        float windSpeed = buffer.getFloat();
        short wingSurface = buffer.getShort();
        float velocity = buffer.getFloat();
        short x = buffer.getShort();
        short y = buffer.getShort();
        short heading = buffer.getShort();
        double latitude = buffer.getDouble();
        double longitude = buffer.getDouble();

        // autopilot debug
        short targetX = buffer.getShort();
        short targetY = buffer.getShort();
        float targetAngle = buffer.getFloat();
        float targetRelativeAngle = buffer.getFloat();
        short targetDistance = buffer.getShort();
        float mainSailTargetAngle = buffer.getFloat();


        SensorData data = new SensorData(
                error,
                stateClifton,
                waypoint,
                windDirection,
                windSpeed,
                wingSurface,
                velocity,
                x,
                y,
                heading,
                latitude,
                longitude,
                targetX,
                targetY,
                targetAngle,
                targetRelativeAngle,
                targetDistance,
                mainSailTargetAngle
                );
        log.debug("Received data: {}", data);
        return data;
    }
}

