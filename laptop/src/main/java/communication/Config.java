package communication;

class Config {
    public static final byte[] MAGIC_HEADER = {0x12, 0x24, 0x36, 0x48, 0x60};
    public static final int MAGIC_LENGTH = MAGIC_HEADER.length;
}
