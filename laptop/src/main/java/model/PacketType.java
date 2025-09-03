package model;

import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

public enum PacketType {
    SENSORS(1, 53),
    COURSE(2, 0), // 0 since we dont actually care on java side parsing, we need only PacketType-ID mappings
    COMMAND(3, 1),
    CONFIG(4, 8 + 8 + 4 + 8 + 1 + 1), // 3x double, 1x float, 2x byte
    ACK(10, 0);

    private final int id;
    private final int payloadSize;
    private static final Map<Integer, PacketType> ID_MAP = Arrays.stream(values())
            .collect(Collectors.toMap(PacketType::getId, Function.identity()));

    PacketType(int id, int payloadSize) {
        this.id = id;
        this.payloadSize = payloadSize;
    }

    public int getId() {
        return id;
    }

    public int getPayloadSize() {
        return payloadSize;
    }

    public static PacketType fromId(int id) {
        PacketType packet = ID_MAP.get(id);
        if (packet == null) throw new IllegalArgumentException("Unknown packet id: " + id);
        return packet;
    }
}
