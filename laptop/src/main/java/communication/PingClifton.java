package communication;

/**
 * Periodically sends an ACK packet to the Clifton indicating that the system is alive.
 * This is needed as there is no regular traffic guaranteed which would indicate a stable connection to the clifton.
 */
public class PingClifton implements Runnable {
    private final Communication communication;
    private volatile boolean running = true;

    public PingClifton(Communication communication) {
        this.communication = communication;
    }

    public void stop() {
        running = false;
    }

    @Override
    public void run() {
        while (running) {
            try {
                communication.sendAck();
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}
