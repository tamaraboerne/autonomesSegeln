package model;

public enum StateClifton {
    IDLE(0b00),
    READY(0b01),
    DRIVING(0b10);

    private final int code;

    StateClifton(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }

    public static StateClifton fromStatusByte(byte statusByte) {
        int stateBits = (statusByte >> 5) & 0b11; // extract bit 5 and 6
        for (StateClifton stateClifton : values()) {
            if (stateClifton.code == stateBits) {
                return stateClifton;
            }
        }
        throw new IllegalArgumentException("Unknown StateClifton in statusByte: " + stateBits);
    }
}