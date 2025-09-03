package communication;

import com.fazecast.jSerialComm.SerialPort;

import java.io.OutputStream;

public class SerialOutputProvider implements OutputProvider {

    private final SerialPort port;

    public SerialOutputProvider(SerialPort comPort) {
        this.port = comPort;
    }

    @Override
    public OutputStream getOutputStream() {
        if (!port.openPort()) return null;
        return port.getOutputStream();
    }
}
