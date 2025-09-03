package communication;

import com.fazecast.jSerialComm.SerialPort;
import model.Command;
import model.ConfigInfo;
import model.Data;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import utils.Point;

import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Communication extends Observable {

    private static final Logger log = LogManager.getLogger(Communication.class);
    private final BlockingQueue<byte[]> senderQueue = new LinkedBlockingQueue<>();
    private final SerialPort comPort;
    private static Communication instance;
    private volatile boolean started = false;

    public Communication(SerialPort port) {
        this.comPort = port;
        port.setBaudRate(38400);
        port.setNumDataBits(8);
        port.setNumStopBits(SerialPort.ONE_STOP_BIT);
        port.setParity(SerialPort.NO_PARITY);
        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
    }

    // only used for tests
    public Communication() {
        comPort = null;
    }

    // Create Singleton instance if it doesn't exist yet
    public static synchronized Communication communication(SerialPort port) {
        if (instance == null) {
            instance = new Communication(port);
        }
        return instance;
    }

    public static Communication communication() {
        if (instance == null) {
            throw new IllegalStateException("Communication has not yet been initialised.");
        }
        return instance;
    }

    /**
     * Starts the communication by initializing and launching the necessary threads
     * for sending, receiving, and periodically pinging data over the serial port to the Clifton.
     *
     * This method must only be called once. If called multiple times, calls will be ignored with a warning.
     *
     * @throws IllegalStateException if the serial port is null or cannot be opened
     */
    public void start() {
        if (started) {
            log.warn("Communication already started. Ignoring duplicate start call.");
            return;
        }
        if (comPort == null) {
            throw new IllegalStateException("Serial port is null. Cannot start communication.");
        }

        if (!comPort.openPort()) {
            throw new IllegalStateException("Failed to open serial port");
        }

        startThread(new XBeeSender(senderQueue, new SerialOutputProvider(comPort)), "XBeeSender");
        startThread(new XBeeReceiver(comPort.getInputStream(), this, false), "XBeeReceiver");
        startThread(new PingClifton(this), "PingClifton");

        started = true;
        log.info("Communication started successfully on port {} with baudrate {}", comPort, comPort.getBaudRate());
    }

    private void startThread(Runnable runnable, String name) {
        Thread thread = new Thread(runnable, name);
        thread.start();
    }

    /**
     * Callback method that is invoked when a packet is received.
     * Notifies all registered observers about the received data from the Clifton.
     *
     * @param seq the sequence number of the received packet
     * @param data the received data contained in the packet
     */
    public synchronized void onPacketReceived(int seq, Data data) {
        setChanged();
        notifyObservers(data);
        log.debug("Notified observers about Packet with seq={}", seq);
    }

    /**
     * Queues a byte array for sending it to the Clifton via the XBee sender.
     *
     * @param bytes the data to send
     */
    private synchronized void send(byte[] bytes) {
        this.senderQueue.add(bytes);
    }

    // only used for simulations
    public static void setInstance(Communication communication) {
        instance = communication;
    }

    public void sendRoute(Point[] route) {
        send(PacketBuilder.createCourse(route));
        log.info("Sent route {}", Arrays.toString(route));
    }

    public void sendCommand(Command command) {
        send(PacketBuilder.createCommand(command));
        log.info("Sent '{}' command", command);
    }

    public void sendConfig(ConfigInfo config){
        send(PacketBuilder.createConfig(config));
        log.info("Sent config {}", config);
    }

    public void sendAck() {
        send(PacketBuilder.createAck());
        log.trace("Sent ACK");
    }

    public BlockingQueue<byte[]> getSenderQueue() {
        return senderQueue;
    }

    public boolean isStarted() {
        return started;
    }
}
