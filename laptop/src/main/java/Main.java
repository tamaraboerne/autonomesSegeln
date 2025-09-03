import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.fazecast.jSerialComm.SerialPort;

import Monitoring.Monitoring;
import communication.Communication;
import frontend.RestApi.Server;
import routeAlgorithm.RouteData;
import utils.CliftonObserver;
import static utils.Config.SERVER_API_PORT;

public class Main {

    private static final Logger log = LogManager.getLogger(Main.class);

    public static void main(String[] args) {

        // init modules communication and frontend
        String portName = args.length > 0 ? args[0] : "ttyUSB0"; // e.g COM3
        SerialPort comPort = SerialPort.getCommPort(portName);
        if (comPort == null) {
            log.error("Port {} not found", portName);
            return;
        }

        try {
            Server server = new Server(SERVER_API_PORT);
            Communication communication = Communication.communication(comPort);
            communication.addObserver(CliftonObserver.clifton());
            communication.addObserver(RouteData.routeData());
            communication.addObserver(Monitoring.monitoring());
            server.start();
            communication.start();
        } catch (Exception e) {
            log.error("Error when starting the application: {}", e.getMessage(), e);
        }
    }
}
