package utils;

import model.SensorData;
import model.StateClifton;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static model.PacketType.SENSORS;

public class TestData {

    public static final byte TYPE_ID = 1;
    public static final byte SEQ = 5;
    public static final byte STATUS_FIELDS = 0b01000001;
    public static final boolean ERROR = false;
    public static final StateClifton STATE_CLIFTON = StateClifton.DRIVING;
    public static final byte WAYPOINT = 1;
    public static final short WIND_DIRECTION = 90;
    public static final float WIND_SPEED = 3.0f;
    public static final short WING_SURFACE = 12;
    public static final float VELOCITY = 3.0f;
    public static final short X = 52;
    public static final short Y = -81;
    public static final short HEADING = 90;
    public static final double LATITUDE = 50.834503;
    public static final double LONGITUDE = 10.699494;

    public static final short TARGET_X = 10;
    public static final short TARGET_Y = 11;
    public static final float TARGET_ANGLE = 11.5f;
    public static final float TARGET_RELATIVE_ANGLE = 12.0f;
    public static final short TARGET_DISTANCE = 50;
    public static final float MAIN_SAIL_TARGET_ANGLE = 12.0f;


    public static byte[] getTestSensorPacketBytes() {
        ByteBuffer packet = ByteBuffer.allocate(SENSORS.getPayloadSize() + 2);
        packet.order(ByteOrder.LITTLE_ENDIAN);
        // heading
        packet.put(TYPE_ID);
        packet.put(SEQ);
        // payload
        packet.put(STATUS_FIELDS);
        packet.putShort(WIND_DIRECTION);
        packet.putFloat(WIND_SPEED);
        packet.putShort(WING_SURFACE);
        packet.putFloat(VELOCITY);
        packet.putShort(X);
        packet.putShort(Y);
        packet.putShort(HEADING);
        packet.putDouble(LATITUDE);
        packet.putDouble(LONGITUDE);

        return packet.array();
    }

    public static SensorData getTestSensorData() {
        return new SensorData(
                ERROR,
                STATE_CLIFTON,
                WAYPOINT,
                WIND_DIRECTION,
                WIND_SPEED,
                WING_SURFACE,
                VELOCITY,
                X, Y,
                HEADING,
                LATITUDE,
                LONGITUDE,
                TARGET_X,
                TARGET_Y,
                TARGET_ANGLE,
                TARGET_RELATIVE_ANGLE,
                TARGET_DISTANCE,
                MAIN_SAIL_TARGET_ANGLE
        );
    }
}
