package communication;

import model.Command;
import model.ConfigInfo;
import model.PacketType;
import utils.Point;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static communication.Config.MAGIC_HEADER;
import static communication.Config.MAGIC_LENGTH;

public class PacketBuilder {

    private static final int HEADER_LENGTH = 2 + MAGIC_LENGTH;

    private static ByteBuffer createHeader(PacketType type, int payloadLength){
        ByteBuffer buffer =  ByteBuffer.allocate(HEADER_LENGTH + payloadLength).order(ByteOrder.LITTLE_ENDIAN);
        for(int i = 0; i < MAGIC_LENGTH; i++){
            buffer.put(MAGIC_HEADER[i]);
        }

        buffer.put((byte) type.getId());
        buffer.put((byte) 0); // seq, todo
        return buffer;
    }

    static byte[] createAck(){
        return createHeader(PacketType.ACK, 0).array();
    }

    static byte[] createCourse(Point[] route){
        // one byte is used for the length field and five bytes are used for each point in the route.
        ByteBuffer buffer = createHeader(PacketType.COURSE, 1 + route.length * 5);
        buffer.put((byte) route.length);
        for (Point point : route) {
            short x = (short) Math.round(point.position.getX());
            short y = (short) Math.round(point.position.getY());
            buffer.putShort(x);
            buffer.putShort(y);
            buffer.put(point.status);
        }

        return buffer.array();
    }

    static byte[] createCommand(Command command){
        ByteBuffer buffer = createHeader(PacketType.COMMAND, PacketType.COMMAND.getPayloadSize());
        buffer.put((byte) command.getValue());

        return buffer.array();
    }

    static byte[] createConfig(ConfigInfo config){
        ByteBuffer buffer = createHeader(PacketType.CONFIG, PacketType.CONFIG.getPayloadSize());
        buffer.putDouble(config.sailSpeedTolerance());
        buffer.putDouble(config.sailSpeedConstant());
        buffer.putFloat(config.flapStep());
        buffer.putDouble(config.steeringConstantP());
        buffer.put(config.defaultWaypointTolerance());
        buffer.put(config.curveWaypointTolerance());

        return buffer.array();
    }


}
