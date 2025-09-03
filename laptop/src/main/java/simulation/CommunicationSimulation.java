package simulation;

import Monitoring.Monitoring;
import communication.Communication;
import communication.XBeeReceiver;
import frontend.RestApi.Server;
import model.PacketType;
import polygon.Probe.CoordinateSystem;
import routeAlgorithm.RouteData;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import utils.CliftonObserver;
import utils.Tuple;
import utils.Vector;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

import static utils.Config.SERVER_API_PORT;

public class CommunicationSimulation {

    private static final Logger log = LogManager.getLogger(CommunicationSimulation.class);

    public static void main(String[] args) throws IOException {
        Server server = new Server(SERVER_API_PORT);
        server.start();
        log.info("Clifton API endpoint is active at http://localhost:{}/api/clifton", SERVER_API_PORT);

        PipedOutputStream outputStream = new PipedOutputStream();
        PipedInputStream inputStream = new PipedInputStream(outputStream);

        Communication communication = new Communication();
        Communication.setInstance(communication);

        CliftonObserver clifton = CliftonObserver.clifton();

        communication.addObserver(clifton);
        communication.addObserver(RouteData.routeData());
        communication.addObserver(Monitoring.monitoring());

        new Thread(new XBeeReceiver(inputStream, communication, true), "XBeeReceiver").start();
        // simulate XBee stream
        new Thread(() -> simulateSensorData(outputStream), "SensorSimulator").start();

        //PathfindingVisualizer.begin();
    }

    private static void simulateSensorData(OutputStream outputStream) {
        Random random = new Random();
        int seq = 0;

        while (true) {
            try {
                byte status = 0b01000001; // ERROR=NO, STATUS=DRIVING; WAYPOINT=1
                short windDirection = (short) random.nextInt(360);
                float windSpeed = random.nextFloat() * 10;
                short wingSurface = (short) random.nextInt(360);
                float velocity = random.nextFloat() * 10;
                short x = 4000; //(short) random.nextInt(5000);
                short y = 1500;// (short) random.nextInt(5000);
                short heading = (short) random.nextInt(360);
                Tuple<Double> gps = CoordinateSystem.coordinateSystem().localVectorToGPS(new Vector(x, y));

                ByteBuffer buffer = ByteBuffer.allocate(2 + PacketType.SENSORS.getPayloadSize());
                buffer.order(ByteOrder.LITTLE_ENDIAN);
                buffer.put((byte) PacketType.SENSORS.getId());
                buffer.put((byte) (seq++ % 256));
                buffer.put(status);
                buffer.putShort(windDirection);
                buffer.putFloat(windSpeed);
                buffer.putShort(wingSurface);
                buffer.putFloat(velocity);
                buffer.putShort(x);
                buffer.putShort(y);
                buffer.putShort(heading);
                buffer.putDouble(gps.first);
                buffer.putDouble(gps.second);

                outputStream.write(buffer.array());
                outputStream.flush();

                Thread.sleep(200);
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

