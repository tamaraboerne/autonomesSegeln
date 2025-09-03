package communication;

import com.fazecast.jSerialComm.SerialPort;
import model.Command;
import model.ConfigInfo;
import model.Data;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import utils.Point;
import utils.Vector;

import java.util.concurrent.BlockingQueue;

import static communication.PacketBuilder.*;
import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.mock;
import static utils.TestData.getTestSensorData;

public class CommunicationTest {

    private Communication communication;

    @BeforeEach
    void setUp() {
        communication = new Communication();
    }

    @Test
    void testCorrectInitialization() {
        SerialPort mockPort = mock(SerialPort.class);
        Communication c1 = Communication.communication(mockPort);
        Communication c2 = Communication.communication();

        assertSame(c1, c2);
    }

    @Test
    void testUninitializedError() {
        assertThrows(IllegalStateException.class, Communication::communication);
    }

    @Test
    void testSendCommandAddsToQueue() {
        Command cmd = Command.Start;
        communication.sendCommand(cmd);

        BlockingQueue<?> queue = communication.getSenderQueue();
        assertFalse(queue.isEmpty());
        byte[] expect = createCommand(Command.Start);
        assertArrayEquals(expect, (byte[]) queue.poll());
    }

    @Test
    void testSendRouteAddsToQueue() {
        Vector v1 = new Vector(1.0f, 2.0f);
        Vector v2 = new Vector(-1.0f, -2.0f);
        Point[] route = new Point[] { new Point(v1, (byte) 1), new Point(v2, (byte) 1) };
        communication.sendRoute(route);

        BlockingQueue<?> queue = communication.getSenderQueue();
        assertFalse(queue.isEmpty());
        byte[] expect = createCourse(route);
        assertArrayEquals(expect, (byte[]) queue.poll());
    }

    @Test
    void testSendConfigAddsToQueue() {
        ConfigInfo config = new ConfigInfo(1.0, 1.0, 1.0f, 0.5, (byte) 1, (byte) 1);
        communication.sendConfig(config);

        BlockingQueue<?> queue = communication.getSenderQueue();
        assertFalse(queue.isEmpty());
        byte[] expect = createConfig(config);
        assertArrayEquals(expect, (byte[]) queue.poll());
    }

    @Test
    void testOnPacketReceivedNotifiesObservers() {
        ObserverMock observer = new ObserverMock();
        Data expectedData = getTestSensorData();
        communication.addObserver(observer);
        communication.onPacketReceived(1, expectedData);

        assertSame(expectedData, observer.received);
    }
}
