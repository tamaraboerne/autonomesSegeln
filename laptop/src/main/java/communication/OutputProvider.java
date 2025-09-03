package communication;

import java.io.OutputStream;

/**
 * Provides an abstraction layer for retrieving an {@link OutputStream} used for sending data.
 *
 * It is useful for testing, as it enables the injection of custom or fake output streams
 * without requiring access to actual serial ports.
 */
public interface OutputProvider {
    OutputStream getOutputStream();
}
