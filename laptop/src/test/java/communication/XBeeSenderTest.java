package communication;

import org.junit.jupiter.api.Test;

import java.io.ByteArrayOutputStream;
import java.util.Arrays;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class XBeeSenderTest {

    @Test
    void testXBeeSender() throws Exception {
        byte[] expected = {1, 2, 3};

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        OutputProvider testProvider = () -> out; // fake outputstream

        BlockingQueue<byte[]> queue = new LinkedBlockingQueue<>();
        queue.add(expected);
        XBeeSender sender = new XBeeSender(queue, testProvider);
        Thread senderThread = new Thread(sender);
        senderThread.start();
        Thread.sleep(100);

        assertArrayEquals(expected, out.toByteArray());
    }

    @Test
    void stressTest() throws Exception {
        final int packetCount = 10;
        final byte[] basePacket = {1, 2, 3};

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        OutputProvider testProvider = () -> out;

        BlockingQueue<byte[]> queue = new LinkedBlockingQueue<>();
        XBeeSender sender = new XBeeSender(queue, testProvider);
        Thread senderThread = new Thread(sender);
        senderThread.start();
    }
}
