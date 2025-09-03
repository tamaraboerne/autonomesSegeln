package communication;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.concurrent.BlockingQueue;

/**
 * Runnable sender that continuously takes byte arrays (packets)
 * from a blocking queue and writes them to an output stream.
 */
public class XBeeSender implements Runnable {

    private static final Logger log = LogManager.getLogger(XBeeSender.class);
    private final BlockingQueue<byte[]> queue;
    private final OutputProvider outputProvider;
    private volatile boolean isRunning;
    private Thread thread;

    public XBeeSender(BlockingQueue<byte[]> queue, OutputProvider outputProvider) {
        this.queue = queue;
        this.outputProvider = outputProvider;
    }

    @Override
    public void run() {
        this.thread = Thread.currentThread();
        isRunning = true;
        OutputStream out = outputProvider.getOutputStream();
        while (isRunning){
            synchronized (this.queue) {
                try {
                    byte[] packet = queue.take();
                    out.write(packet);
                    out.flush();
                    log.debug("Send data to clifton: {}", Arrays.toString(packet));
                } catch (InterruptedException e) {
                    log.info("Sender thread interrupted");
                    break;
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    /**
     * Signals the sender thread to stop running.
     * The thread may need to be interrupted because queue.take() could block the thread.
     */
    public void stop() {
        isRunning = false;
        if (thread != null) {
            thread.interrupt();
        }
    }
}
