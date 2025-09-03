package frontend.RestApi;

import Monitoring.Monitoring;
import com.sun.net.httpserver.HttpServer;
import frontend.RestApi.handler.ApiHandler;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import utils.CliftonObserver;

import java.io.IOException;
import java.net.InetSocketAddress;

public class Server {

    private static final Logger log = LogManager.getLogger(Server.class);
    private final int port;
    private final CliftonObserver cliftonObserver;
    private final Monitoring monitoring;

    public Server(int port) {
        this.port = port;
        this.cliftonObserver = CliftonObserver.clifton();
        this.monitoring = Monitoring.monitoring();
    }

    public void start() throws IOException {
        HttpServer server = HttpServer.create(new InetSocketAddress(port), 0);
        var apiHandler = ApiHandler.apiHandler(cliftonObserver, monitoring);

        // Adding API endpoints to listen to
        server.createContext("/api/clifton", apiHandler);
        server.createContext("/api/start", apiHandler);
        server.createContext("/api/stop", apiHandler);
        server.createContext("/api/route", apiHandler);
        server.createContext("/api/health", apiHandler);
        server.createContext("/api/debug/clifton", apiHandler);
        server.createContext("/api/debug/backend", apiHandler);
        server.setExecutor(null);
        server.start();

        log.info("Server started on http://localhost:{}", port);
    }
}
