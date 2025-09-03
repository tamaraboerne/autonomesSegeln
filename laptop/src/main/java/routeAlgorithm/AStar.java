package routeAlgorithm;

import communication.Communication;
import javafx.application.Platform;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import polygon.Probe.CoordinateSystem;
import polygon.Probe.convexe.Polygon;
import utils.*;
import utils.Vector;

import java.util.*;



public class AStar {

    private static final Logger log = LogManager.getLogger(AStar.class);

    private static Vector[] coordinates;

    public static Point[] latestPath;
    public static Point[] latestPathSmoothed;


    /**  Method to start the Search Algorithm, called only by the Destination Position (Getting the other Parameters from the Clifton itself)
     *
     * @param destination End position (GPS-Coordiantes)
     * @return Result of the search containing the path to the destination if possible, contains a String of error Messages if it fails
     */
    public static RouteResult searchGPS(Tuple<Double> destination){

        // Get data from the RoutaData observer and call searchGPS with all parameters
        return searchGPS(CoordinateSystem.coordinateSystem().localVectorToGPS(RouteData.routeData().currentPosition), destination, RouteData.routeData().getWindDirection(), RouteData.routeData().getWindSpeed(), RouteData.routeData().orientationClifton);

    }


    /** Overloaded Method for Debug purposes, called by the regular searchGPS Method
     *
     *
     * @param start  Start position (GPS-Coordinates)
     * @param destination  End position (GPS-Coordinates)
     * @param windAngle  relative to the Clifton (calculates Windangle relative to the word Coordinate System)
     * @param windSpeed  Windspeed in m/s
     * @param orientation  Orientation of the Cliftion (north is 0°, east is 90°)
     * @return Returns a Result, depending if a Path could be found, containing a String with occuring Error messages
     */
    public static RouteResult searchGPS(Tuple<Double> start, Tuple<Double> destination, float windAngle, float windSpeed, float orientation){
        // Covert angles
        windAngle = RouteData.convertAngle(windAngle);
        orientation = RouteData.convertAngle(orientation);

        log.info("Start Position: {}; End Position: {}; windAngle: {}; windSpeed: {}; Orientation: {}", start.toString(), destination, windAngle, windSpeed, orientation);

        State startState = new State(CoordinateSystem.coordinateSystem().gpsToLocalVector(start), orientation);

        boolean success = true;
        String message = "";
        // Test for errors
        if(!Polygon.sailingArea().containsPoint(startState.position)){
            success = false;
            message += "The start point is not in the sailing area!\n";
        }
        if(!Polygon.sailingArea().containsPoint(CoordinateSystem.coordinateSystem().gpsToLocalVector(destination))){
            success = false;
            message += "The end point is not in the sailing area!\n";
        }

        if(success){
            Point[] path = search(startState, CoordinateSystem.coordinateSystem().gpsToLocalVector(destination), windAngle, windSpeed, orientation);
            if(path.length == 0){
                success = false;
                message += "No path found!\n";
            }
            if(!validToWind(orientation, windAngle)){
                message += "Clifton's rotation is invalid for wind direction!\n";
            }
            if(windSpeed < Config.MIN_WIND_SPEED){
                message += "The wind speed is too low!\n";
            }

            Tuple[] returnPath = new Tuple[path.length];
            for(int i = 0; i < path.length; i++) {
                returnPath[i] = CoordinateSystem.coordinateSystem().localVectorToGPS(path[i].position);
            }

            log.info("RouteResult: success: {}, message: {}, path: {}", success, message, returnPath);

            return new RouteResult(returnPath, success, message);
        }

        return new RouteResult(new Tuple[0], success, message);
    }


    /** Search Method, starts the Algorithm, returns an Array of Points when it finds a Path, returns an empty Array of Points otherwise.
     * When a path is found, the returned Path will be smoothed i.e. Curve entry and exit Points will be added.
     *
     * @param startState  State from which the Clifton starts (position and orientation)
     * @param destination  Destination position
     * @param windAngle  Absolute to the World Coordinate System (0° is East, 90° is North)
     * @param windSpeed  Wind in m/s
     * @param orientation  Orientation of the Cliftion in the same Format as the windAngle
     * @return Array of Points
     */
    public static Point[] search(State startState, Vector destination, float windAngle, float windSpeed, float orientation){
        coordinates = createCoordinates(startState.position, destination);
        //Platform.runLater(() -> PathfindingVisualizer.initAlgorithm(startState.position, destination, coordinates, windAngle));

        // Create a frontier with either a regular or an Optimized comparator
        TreeSet<Node> frontier = new TreeSet<>(new NodeComparator(destination, windAngle));

        // Create starting node without parent
        Node startNode = new Node(startState, null);
        frontier.add(startNode);
        // Create HashMap to memorize reached states with their best nodes
        //Map<State, Node> reached = new HashMap<>();
        Map<Vector, Node> reached = new HashMap<>();

        while (!frontier.isEmpty()){
            // next Node
            Node curNode = frontier.pollFirst();
            //Platform.runLater(() -> PathfindingVisualizer.drawNode(curNode.state.position,"expand"));
            // Track states entered and maximal frontier size

            // Goal check
            if(curNode.state.position.equals(destination)) {
                Point[] path = new Point[curNode.depth()];
                Node returnNode = curNode;
                for(int i = curNode.depth() - 1; i >= 0; i--) {
                    path[i] = new Point(returnNode.state.position, (byte) 0);
                    returnNode  = returnNode.parentNode;
                }
                latestPath = path;
                latestPathSmoothed = smoothPath(path);
                return latestPathSmoothed;
            }


            /*
            Iterate through all possible actions (nodes for reachable states) for the current state
            and add the nodes to frontier if the states haven't been reached before
            or have lower path costs than previous nodes of a reached state.
             */
            //reached.put(curNode.state.position, curNode);
            int newPositions = 0;
            for(Node childNode : actions(curNode, windAngle)){
                //if(!reached.containsKey(childNode.state)){
                if(!reached.containsKey(childNode.state.position)){
                    //reached.put(childNode.state, childNode);
                    reached.put(childNode.state.position, childNode);
                    frontier.add(childNode);
                    newPositions++;
                }
                //else if(childNode.pathCosts < reached.get(childNode.state).pathCosts){
                else if(childNode.pathCosts < reached.get(childNode.state.position).pathCosts){
                    // Removes nodes in frontier of the same state to replace them with nodes with better paths
                    //frontier.remove(reached.get(childNode.state));
                    frontier.remove(reached.get(childNode.state.position));
                    //reached.put(childNode.state, childNode);
                    reached.put(childNode.state.position, childNode);
                    //System.out.println("better path costs!");
                    frontier.add(childNode);
                }
            }
            //log.debug("New positions: " + newPositions);
            //log.debug("Current frontier size: " + frontier.size());
        }

        return new Point[0];
    }

    public static Iterable<Node> actions(Node currentNode, float windAngle){
        List<Node> possibleActions = new ArrayList<>();

        for(Vector candidatePosition : coordinates){
            if(isCandidateValid(currentNode.state.position, candidatePosition, windAngle)) {
                float angleOfArrival = new Vector(candidatePosition.getX() - currentNode.state.position.getX(), candidatePosition.getY() - currentNode.state.position.getY()).angle();
                possibleActions.add(new Node(new State(candidatePosition,  angleOfArrival), currentNode));
                //Platform.runLater(() -> PathfindingVisualizer.drawNode(candidatePosition,"valid"));
            }
            else {
                //Platform.runLater(() -> PathfindingVisualizer.drawNode(candidatePosition,"invalid"));
            }
        }

        //log.debug("Possible actions: " + possibleActions);

        return possibleActions;
    }

    /** Checks if a Candidate Position is valid i.e. reachable through Wind and Territory.
     *
     * @param currentPosition current position of the clifton in the algorithm
     * @param candidatePosition potential position of the clifton to be validated
     * @param windAngle wind angle that determines if a candidate position is valid
     * @return true if valid
     */
    public static boolean isCandidateValid(Vector currentPosition, Vector candidatePosition, float windAngle) {
        // Caluclate if angleToWind is possible
        Vector direction = new Vector(candidatePosition.getX() - currentPosition.getX(), candidatePosition.getY() - currentPosition.getY());
        float angle = direction.angle();
        return validToWind(angle, windAngle) && Polygon.sailingArea().isPathInTerritory(currentPosition, candidatePosition);
    }

    /** Checks if the Clifton can go in a specific direction based to the wind.
     *
     * @param angle Angle of the Clifton
     * @param windAngle Angle of the Wind
     * @return true, when the Clifton will be able to go that direction
     */
    public static boolean validToWind(float angle, float windAngle) {
        float angleToWind = ((angle - windAngle) + 360) % 360;
        if(angleToWind > 180) angleToWind = 360 - angleToWind; // Project Angles between 180 and 360 to angles of 180 to 0

        // Return false if the angle to wind angle is outside of the bounds
        if(angleToWind < Config.MIN_ANGLE_OF_ATTACK || angleToWind > Config.MAX_ANGLE_OF_ATTACK) {
            return false;
        }
        return true;
    }

    /** Creates all Coordinates inside the Sailing Area, the Grid is aligned to the Destination.
     *
     * @param startPosition start position of the clifton
     * @param destinationPosition destination of the clifton
     * @return all coordinates in the sailing area
     */
    public static Vector[] createCoordinates(Vector startPosition, Vector destinationPosition) {
        ArrayList<Vector> tempList = new ArrayList<>();
        if(Config.grid.equals("static")) {

            // Calculate offsets to align the grid with the destination
            float xOffset = destinationPosition.getX() % Config.GRID_CELL_SIZE;
            float yOffset = destinationPosition.getY() % Config.GRID_CELL_SIZE;


            float xOffsetArea = Polygon.sailingArea().minX() % Config.GRID_CELL_SIZE;
            float yOffsetArea = Polygon.sailingArea().minY() % Config.GRID_CELL_SIZE;

            log.info("xOffset: " + xOffset + ", yOffset: " + yOffset);
            log.info("xOffsetArea: " + xOffsetArea + ", yOffsetArea: " + yOffsetArea);


            for(int x = (int)(Polygon.sailingArea().minX() - xOffsetArea - Config.GRID_CELL_SIZE); x + xOffset <= Polygon.sailingArea().maxX(); x += Config.GRID_CELL_SIZE) {
                for (int y = (int)(Polygon.sailingArea().minY() - yOffsetArea - Config.GRID_CELL_SIZE); y + yOffset <= Polygon.sailingArea().maxY(); y += Config.GRID_CELL_SIZE) {
                    Vector candidatePosition = new Vector(x + xOffset, y + yOffset);
                    if(Polygon.sailingArea().containsPoint(candidatePosition)) {
                        tempList.add(candidatePosition);
                    }
                }
            }


        } else if(Config.grid.equals("dynamic")) {
            // Calculate offsets to align the grid with the destination
            float xOffset = destinationPosition.getX() % Config.GRID_CELL_SIZE;
            float yOffset = destinationPosition.getY() % Config.GRID_CELL_SIZE;

            float xOffsetArea = Polygon.sailingArea().minX() % Config.GRID_CELL_SIZE;
            float yOffsetArea = Polygon.sailingArea().minY() % Config.GRID_CELL_SIZE;

            for(int x = (int)(Polygon.sailingArea().minX() - xOffsetArea - Config.GRID_CELL_SIZE); x + xOffset <= Polygon.sailingArea().maxX(); x += Config.GRID_CELL_SIZE) {
                for (int y = (int)(Polygon.sailingArea().minY() - yOffsetArea - Config.GRID_CELL_SIZE); y + yOffset <= Polygon.sailingArea().maxY(); y += Config.GRID_CELL_SIZE) {
                    Vector candidatePosition = new Vector(x + xOffset, y + yOffset);
                    if(Polygon.sailingArea().containsPoint(candidatePosition)) {
                        tempList.add(candidatePosition);
                    }
                }
            }
            ArrayList<Integer> radiiPercent = new ArrayList<>(Arrays.asList(1, 2, 3 ,5, 10, 15, 25, 35, 50));
            float distance = startPosition.distanceTo(destinationPosition);

            for(Integer radiusPercent : radiiPercent) {
                float radius = ((float) radiusPercent / 100) * distance;
                float circumference = (float) (2 * Math.PI * radius);
                int numPoints = (int) (circumference / Config.DYNAMIC_STEP_SIZE);
                for(int i = 0; i < numPoints; i++) {
                    double angle = 2 * Math.PI * i / numPoints;
                    double x = radius * Math.cos(angle);
                    double y = radius * Math.sin(angle);
                    Vector startCandidate = new Vector((int) x + startPosition.getX(), (int) y + startPosition.getY());
                    if(Polygon.sailingArea().containsPoint(startCandidate)) {
                        tempList.add(startCandidate);
                    }
                    Vector destinationCandidate = new Vector((int) x + destinationPosition.getX(), (int) y + destinationPosition.getY());
                    if(Polygon.sailingArea().containsPoint(destinationCandidate)) {
                        tempList.add(destinationCandidate);
                    }
                }
            }
        }
        //log.debug("tempList: " + tempList);
        log.info("Destination in List: " + tempList.contains(destinationPosition));


        return tempList.toArray(new Vector[0]);
    }


    /**  Returns the Smoothed Path, containing the Curve entry and exit points
     *
     * @param path Not smoothed path containing the original checkpoints
     * @return Path with modified checkpoints
     */
    public static Point[] smoothPath(Point[] path) {
        // Loop ignores first and last entry
        if(path.length <= 2) {
            return path;
        }

        Point[] smoothedPath = new Point[(path.length * 2) - 2];
        smoothedPath[0] = path[0];
        smoothedPath[smoothedPath.length - 1] = path[path.length - 1];
        for(int i = 1; i < path.length - 1; i++) {
            float angle = path[i - 1].position.sub(path[i].position).angleDifferenceTo(path[i + 1].position.sub(path[i].position)) / 2f;
            //log.debug("angle: " + angle);
            float distanceToCurrentPoint = (float) Math.abs(Config.TURNING_RADIUS / (Math.tan(Math.toRadians(angle))));
            //log.debug("distanceToCurrentPoint: " + distanceToCurrentPoint);
            smoothedPath[(i * 2) - 1] =  new Point(path[i - 1].position.sub(path[i].position).normalize().scale(distanceToCurrentPoint).add(path[i].position), (byte)1);
            smoothedPath[i * 2] = new Point(path[i + 1].position.sub(path[i].position).normalize().scale(distanceToCurrentPoint).add(path[i].position), (byte)2);
        }

        return smoothedPath;
    }

    /**
     * Sends the most recently calculated and smoothed path to the clifton
     */
//    public static void sendPathToClifton() {
//        try {
//            Communication.communication().setRoute(latestPathSmoothed);
//            log.info("Path sent to clifton successfully");
//        } catch (Exception e) {
//            log.error("Error sending path to clifton: {}", e.getMessage());
//        }
//    }
}

