package Monitoring;

import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import static communication.Communication.communication;
import model.Command;
import model.SensorData;
import routeAlgorithm.AStar;
import utils.Circle;
import utils.Config;
import utils.Point;
import utils.Rectangle;
import utils.Vector;

/**
 * Monitoring System that tracks the Clifton's position relative to a planned route.
 * Implements Observer pattern to receive sensor data updates
 */
public class Monitoring implements Observer  {

    private static final Logger log = LogManager.getLogger(Monitoring.class);
    private static Monitoring instance;

    private int counter = 0;

    private Vector currentPosition;
    private boolean isDriving = false;
    private Point[] route;



    private List<Rectangle> pathRectangles = new ArrayList<>();
    private List<Circle> pathCircle = new ArrayList<>();

    private Monitoring() {
        log.info("Monitoring system initialized");
    }

    /**
     * Returns the singleton instance of Monitoring
     * @return The single instance of Monitoring system
     */
    public static Monitoring monitoring() {
        if (instance == null) {
            instance = new Monitoring();
            log.info("Created new Monitoring instance");
        }
        return instance;
    }

    /**
     * Observer update method called when sensor data changes
     * @param o The observable object
     * @param arg The updated sensor data
     */
    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof SensorData) {
            var x = (float) ((SensorData) arg).x();
            var y = (float) ((SensorData) arg).y();
            this.currentPosition = new Vector(x, y);
            log.debug("Current position: {}", this.currentPosition);
        }

        if (isDriving && !isValidPosition(currentPosition)) { // Sofortige Prüfung
            log.error("🚨 KRITISCHE ABWEICHUNG: {}", currentPosition);
            stopMonitoring();
        }

//        // If clifton is driving
//        if(isDriving) {
//            counter++;
//            if((counter % 10) == 0) {// Is position valid
//                counter = 0;
//                if(!isValidPosition(currentPosition)) {
//                    log.info("Invalid position detected at {}", currentPosition);
//                    stopMonitoring();
//                } else {
//                    log.debug("Position validation successful at {}", currentPosition);
//                }
//            }
//
//        }
    }

    /**
     * Sends start command to Clifton
     */
    public void startClifton() {
        log.info("Sending Start command to Clifton");
        communication().sendCommand(Command.Start);
    }

    /**
     * Sends stop command to Clifton
     */
    public void stopClifton() {
        log.info("Sending Stop command to Clifton");
        communication().sendCommand(Command.Stop);
    }

    /**
     * Starts the monitoring system and begins tracking
     */
    public void startMonitoring() {

        // set flag isDriving = True
        log.info("Starting monitoring system");
        isDriving = true;
        log.info("Driving flag set to: {}", isDriving);

        // Fetch Route
        route = AStar.latestPathSmoothed;
        if (route == null || route.length == 0) {
            log.error("No route available for monitoring");
            return;
        }
        log.info("Route loaded with {} points", route.length);

        // create Objects (Formen)
        createShapes();
        log.info("Created {} circles and {} rectangles for path monitoring",
                pathCircle.size(), pathRectangles.size());

        // Send Pfad
        communication().sendRoute(route);
        log.info("Route sent to communication system");

        // send Start
        startClifton();
        log.info("Monitoring system started successfully");
    }

    /**
     * Stops the monitoring system and Clifton's movement
     */
    public void stopMonitoring() {
        log.info("Stopping monitoring system");
        isDriving = false;
        //communication().deleteObserver(this);
        log.info("Driving flag set to: {}", isDriving);
        stopClifton();
        log.info("Monitoring system stopped");
    }

    /**
     * Creates geometric shapes along the path for position validation
     */
    public void createShapes() {
        log.info("Creating shapes for path monitoring");

        // create Circle
        for (Point point : route) {
            pathCircle.add(new Circle(point.position, Config.PATH_TOLERANCE));
        }
        log.debug("Created {} path circles", pathCircle.size());

        // for each pair of points, create rectangle
        for (int i = 0; i < route.length - 1; i++) {
            // i && i + 1
            pathRectangles.add(new Rectangle(route[i].position, route[i + 1].position));
        }
        log.debug("Created {} path rectangles", pathRectangles.size());
    }

    /**
     * Validates if a position is within the allowed path boundaries
     * @param position The current position to validate
     * @return true if position is valid, false otherwise
     */
     public boolean isValidPosition(Vector position) {
        // 1. Prüfe zuerst die Kreise (um Wegpunkte)
         for (Circle c : pathCircle) {
            if (c.containsPoint(position)) {
                log.trace("Position {} validiert durch Kreis {}", position, c);
                return true;
            }
        }
            // 2. Prüfe die Rechtecke (zwischen Wegpunkten)
        for (Rectangle r : pathRectangles) {
            if (r.containsPoint(position)) {
                log.trace("Position {} validiert durch Rechteck {}", position, r);
                return true;
            }
        }
        log.debug("Position {} ist UNGÜLTIG (außerhalb der Route)", position);
        return false;
    }

    /*public synchronized void startMonitoring() {
        if (isMonitoringActive) {
            log.info("Monitoring already running! {}",isMonitoringActive);
            return;
        }

        if (!fetchPath()) {
            log.error("Failed to start: No route available");
            stopMonitoring();
            return;
        }

        if (!sendPathToClifton()) {
            log.error("Failed to start: Sending failed");
            stopMonitoring();
            return;
        }

//        if (!sendPathToSegler()) {
//            // TODO: retry?
//            log.error("Failed to start: Sending failed");
//            stopMonitoring();
//            return;
//        }


        isMonitoringActive = true;

        new Thread(this::monitoringLoop).start();
        // Send the 'Start' command to Clifton once monitoring has been established.
        communication().setCommand(Command.Start);
    }*/

//    /**
//     * Sends the most recently calculated and smoothed path to the clifton
//     */
//    public static void sendPathToClifton() {
//        try {
//            Communication.communication().setRoute(latestPathSmoothed);
//            log.info("Path sent to clifton successfully");
//        } catch (Exception e) {
//            log.error("Error sending path to clifton: {}", e.getMessage());
//        }
//    }


    /**
     * * Main monitoring loop
     */
    /*private void monitoringLoop() {

        log.info("Started monitoring loop");
        while (isMonitoringActive && currentSegmentIndex < cachedPath.length - 1) {

            if (currentPosition != null && !currentPosition.equals(previousPosition)) {
                Point segmentStart = cachedPath[currentSegmentIndex];
                Point segmentEnd = cachedPath[currentSegmentIndex + 1];

                // Vektoren for polygon check
                Vector segmentStartVec = segmentStart.position;
                Vector segmentEndVec = segmentEnd.position;

                // 2. implement course monitoring
//                boolean isValid = Polygon.isPointNearSegmentValid(
//                        segmentStartVec,
//                        segmentEndVec,
//                        currentPosition,
//                        Polygon.sailingArea(),
//                        TOLERANCE_RADIUS
//                );

                if (!Polygon.isPointNearSegmentValid(segmentStartVec, segmentEndVec, currentPosition, Polygon.sailingArea(), TOLERANCE_RADIUS)) {
                    handleDeviation();
                    if (currentPosition.distanceTo(segmentEndVec) < TOLERANCE_RADIUS) {
                        stopMonitoring();
                    }
                }
                advanceToNextSegment();
            } else log.trace("currentPosition is unchanged or null, skipping loop iteration");

            previousPosition = currentPosition;

            try {
                Thread.sleep(500);  // short Pause between
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                log.error("InterruptedException while sleeping: {}", e.getMessage());
                log.warn("Monitoring stopped");
                break;
            }
        }

        // Check if route was completed
        if (isMonitoringActive && currentSegmentIndex >= cachedPath.length - 1) {
            log.info("Monitoring: All route segments completed successfully!");
            stopMonitoring();
        } else if (!isMonitoringActive && currentSegmentIndex < cachedPath.length - 1) {
            log.info("Monitoring: Stopped manually or due to an error.");
        }
        log.info("Monitoring: loop ended");
        stopMonitoring();
    }*/
}