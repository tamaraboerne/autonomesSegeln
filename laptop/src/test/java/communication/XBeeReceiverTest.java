package communication;

import model.Data;
import model.SensorData;
import org.junit.jupiter.api.Test;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import static org.junit.jupiter.api.Assertions.*;
import static utils.TestData.*;

public class XBeeReceiverTest {

    @Test
    void testSensorPacketHandling() throws InterruptedException {
        byte[] packet = getTestSensorPacketBytes();

        InputStream simulatedInput = new ByteArrayInputStream(packet);
        CommunicationMock communication = new CommunicationMock();

        XBeeReceiver receiver = new XBeeReceiver(simulatedInput, communication, true);
        Thread receiverThread = new Thread(receiver);
        receiverThread.start();
        Thread.sleep(100);

        Data data = communication.getReceivedData();
        assertNotNull(data, "Data should not be null");
        assertInstanceOf(SensorData.class, data, "Data should be of type SensorData");

        SensorData sensorData = (SensorData) data;
        assertEquals(ERROR, sensorData.error());
        assertEquals(STATE_CLIFTON, sensorData.stateClifton());
        assertEquals(WAYPOINT, sensorData.waypoint());
        assertEquals(WIND_DIRECTION, sensorData.windDirection());
        assertEquals(WIND_SPEED, sensorData.windSpeed());
        assertEquals(VELOCITY, sensorData.velocity());
        assertEquals(X, sensorData.x());
        assertEquals(Y, sensorData.y());
        assertEquals(HEADING, sensorData.heading());
        assertEquals(LATITUDE, sensorData.latitude());
        assertEquals(LONGITUDE, sensorData.longitude());
    }
}
