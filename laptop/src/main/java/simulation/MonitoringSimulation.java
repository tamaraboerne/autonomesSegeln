package simulation;


import Monitoring.Monitoring;
import communication.Communication;
import frontend.RestApi.Server;
import model.SensorData;
import routeAlgorithm.AStar;
import utils.Point;
import utils.Vector;

import java.io.IOException;

import static utils.Config.SERVER_API_PORT;
import static utils.TestData.*;

public class MonitoringSimulation {

    static Point[] testRoute = new Point[] {
            // Format: new Point(Vector(x,y), byte-Flag)
            // Flag: 0=Normalpunkt, 1=Kurvenstart, 2=Kurvenende
            new Point(new Vector(0, 0), (byte) 0),      // Startpunkt
            new Point(new Vector(15, 5), (byte) 1),     // Erste Kurve
            new Point(new Vector(30, 10), (byte) 2),    // Kurvenende
            new Point(new Vector(45, 20), (byte) 1),    // Schärfere Kurve
            new Point(new Vector(50, 25), (byte) 2),
            new Point(new Vector(60, 30), (byte) 0),     // Zielpunkt
    };

    static Vector[] simulatedPositions = {
            // Startpunkt (exakt auf Route)
            new Vector(0, 0),

            // Bewegung zum 1. Wegpunkt (leichte Abweichung nach rechts)
            new Vector(5, 2),    // 80% Kurshaltung
            new Vector(10, 3),   // 70% Kurshaltung (simulierter Seitenwind)
            new Vector(100, 500),     // Exakt am Kurvenstart

            // Kurvenfahrt (mit Übersteuern)
            new Vector(18, 6),   // Leicht außerhalb der Kurvenlinie
            new Vector(22, 8),   // Korrigiert zurück
            new Vector(30, 10),    // Exakt am Kurvenende

            // Geradeaus mit Winddrift
            new Vector(35, 12),  // Wind driftet nach links
            new Vector(40, 15),    // Manuell korrigiert
            new Vector(45, 20),    // Exakt am nächsten Kurvenstart

            // Letzte Kurve (ideale Linie)
            new Vector(50, 23),
            new Vector(55, 26),
            new Vector(60, 30)     // Ziel erreicht
    };

    public static void main(String[] args) throws IOException {

        Server server = new Server(SERVER_API_PORT);
        server.start();

        Communication communication = new Communication();
        Communication.setInstance(communication);

        Monitoring monitoring = Monitoring.monitoring();
        communication.addObserver(monitoring);

        AStar.latestPathSmoothed = testRoute;
        monitoring.startMonitoring();

        for (int i = 0; i < simulatedPositions.length; i++) {
            SensorData data = new SensorData(
                    ERROR,
                    STATE_CLIFTON,
                    (byte) i,
                    WIND_DIRECTION,
                    WIND_SPEED,
                    WING_SURFACE,
                    VELOCITY,
                    (short) simulatedPositions[i].getX(),
                    (short) simulatedPositions[i].getY(),
                    HEADING,
                    LONGITUDE,
                    LATITUDE,
                    TARGET_X,
                    TARGET_Y,
                    TARGET_ANGLE,
                    TARGET_RELATIVE_ANGLE,
                    TARGET_DISTANCE,
                    MAIN_SAIL_TARGET_ANGLE
            );

            // Send new position to monitoring
            communication.onPacketReceived(1, data);

            try {
                Thread.sleep(2500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
