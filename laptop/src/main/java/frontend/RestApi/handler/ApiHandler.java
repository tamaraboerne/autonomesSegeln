package frontend.RestApi.handler;

import Monitoring.Monitoring;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import model.ConfigInfo;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import utils.CliftonObserver;
import utils.Tuple;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

import static communication.Communication.communication;
import static routeAlgorithm.AStar.searchGPS;

public class ApiHandler implements HttpHandler {

    private static final Logger log = LogManager.getLogger(ApiHandler.class);

    private final CliftonObserver cliftonObserver;

    private final Monitoring monitoring;

    private ApiHandler(CliftonObserver cliftonObserver, Monitoring monitoring) {
        this.cliftonObserver = cliftonObserver;
        this.monitoring = monitoring;
    }

    // Factory method to create ApiHandler
    public static ApiHandler apiHandler(CliftonObserver cliftonObserver, Monitoring monitoring) {
        return new ApiHandler(cliftonObserver, monitoring);
    }

    /**
     * Method to handle incoming api requests
     * @param exchange The HttpExchange object containing request data
     * @throws IOException For I/O errors
     */
    @Override
    public void handle(HttpExchange exchange) throws IOException {
        // Set CORS headers for cross-origin requests
        exchange.getResponseHeaders().add("Access-Control-Allow-Origin", "*");
        exchange.getResponseHeaders().add("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        exchange.getResponseHeaders().add("Access-Control-Allow-Headers", "Content-Type, Authorization");

        // Handle preflight OPTIONS request immediately
        if ("OPTIONS".equalsIgnoreCase(exchange.getRequestMethod())) {
            exchange.sendResponseHeaders(200, -1);
            return;
        }

        // Extract HTTP method and path
        var method = exchange.getRequestMethod();
        var path = exchange.getRequestURI().getPath();

        // Route based on request path
        switch (path) {
            case "/api/start":
                if ("POST".equals(method)) {
                    try {
                        handlePostStart(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/stop":
                if ("POST".equals(method)) {
                    try {
                        handlePostStop(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/clifton":
                if ("GET".equals(method)) {
                    try {
                        handleGetClifton(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/route":
                if ("POST".equals(method)) {
                    try {
                        handlePostRoute(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/debug/clifton":
                if ("POST".equals(method)) {
                    try {
                        handlePostDebugClifton(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/debug/backend":
                if ("POST".equals(method)) {
                    try {
                        handlePostDebugBackend(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            case "/api/health":
                if ("GET".equals(method)) {
                    try {
                        handleGetHealth(exchange);
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                else {
                    try {
                        sendError(exchange, 405, "Method Not Allowed");
                    } catch (JSONException e) {
                        throw new RuntimeException(e);
                    }
                }
                break;
            default:
                try {
                    sendError(exchange, 404, "Not Found");
                } catch (JSONException e) {
                    throw new RuntimeException(e);
                }
        }
    }

    /**
     * Handles POST /api/start requests.
     * Starts monitoring.
     */
    private void handlePostStart(HttpExchange exchange) throws IOException, JSONException {
        log.info("Received request: POST /api/start");
        monitoring.startMonitoring();

        var jsonResponse = new JSONObject();
        jsonResponse.put("success", true);
        sendResponse(exchange, 200, jsonResponse.toString());
    }

    /**
     * Handles POST /api/stop requests.
     * Stops monitoring and driving.
     */
    private void handlePostStop(HttpExchange exchange) throws IOException, JSONException {
        log.info("Received request: POST /api/stop");
        monitoring.stopMonitoring();

        var jsonResponse = new JSONObject();
        jsonResponse.put("success", true);
        sendResponse(exchange, 200, jsonResponse.toString());
    }

    /**
     * Handles POST /api/debug/clifton requests.
     * Sets debug parameters for the Clifton on board.
     */
    private void handlePostDebugClifton(HttpExchange exchange) throws IOException, JSONException {
        log.info("Received request: POST /api/debug/clifton");
        var jsonInput = parseRequestBody(exchange);

        // Clifton debug parameters
        var sailSpeedTolerance = jsonInput.getDouble("sailSpeedTolerance");
        var sailSpeedConstant = jsonInput.getDouble("sailSpeedConstant");
        var flapStepSize = jsonInput.getDouble("flapStep");
        var steeringConstantP = jsonInput.getDouble("steeringConstantP");
        var defaultWaypointTolerance = jsonInput.getInt("defaultWaypointTolerance");
        var curveWaypointTolerance = jsonInput.getInt("curveWaypointTolerance");

        var configInfo = new ConfigInfo(sailSpeedTolerance, sailSpeedConstant, (float) flapStepSize, steeringConstantP, (byte) defaultWaypointTolerance, (byte) curveWaypointTolerance);
        communication().sendConfig(configInfo);

        var jsonResponse = new JSONObject();
        jsonResponse.put("success", true);
        sendResponse(exchange, 200, jsonResponse.toString());
    }

    /**
     * Handles POST /api/debug/backend requests.
     * Sets debug parameters for the clifton observer.
     */
    private void handlePostDebugBackend(HttpExchange exchange) throws IOException, JSONException {
        log.info("Received request: POST /api/debug/backend");
        var jsonInput = parseRequestBody(exchange);

        // Backend/Frontend debug parameters
        var debugStatus = jsonInput.getBoolean("status");
        var debugWindSpeed = jsonInput.getDouble("windSpeed");
        var debugWindDirection = jsonInput.getInt("windDirection");

        cliftonObserver.setDebug(debugStatus);
        cliftonObserver.updateWindSpeed(debugWindSpeed);
        cliftonObserver.updateWindDirection((short) debugWindDirection);

        var jsonResponse = new JSONObject();
        jsonResponse.put("success", true);
        sendResponse(exchange, 200, jsonResponse.toString());
    }

    /**
     * Handles POST /api/route requests.
     * Calculates a route based on provided coordinates using the a-star algorithm.
     */
    private void handlePostRoute(HttpExchange exchange) throws IOException, JSONException {
        log.info("Received request: POST /api/route");
        var jsonInput = parseRequestBody(exchange);

        var inputLat = jsonInput.getDouble("lat");
        var inputLng = jsonInput.getDouble("lon");
        var destination = new Tuple<>(inputLat, inputLng);

        // Debug data, only used in debug mode
        var location = new Tuple<>((double) cliftonObserver.getLatitude(), (double) cliftonObserver.getLongitude());
        var windSpeed = cliftonObserver.getWindSpeed();
        var windDirection = cliftonObserver.getWindDirection();
        var heading = cliftonObserver.getHeading();

        var routeResult = cliftonObserver.getDebug()
                ? searchGPS(location, destination, windDirection, (float) windSpeed, heading)
                : searchGPS(destination);

        // Return error, if no route was found
        if (!routeResult.success) {
            log.warn("Route calculation failed: {}", routeResult.message);
            sendError(exchange, 500, routeResult.message);
        }

        // Parse route into JSONObject
        var jsonRoute = new JSONArray();
        for (var point : routeResult.route) {
            var coord = new JSONArray();
            coord.put(point.first);
            coord.put(point.second);

            jsonRoute.put(coord);
        }

        var jsonResponse = new JSONObject();
        jsonResponse.put("route", jsonRoute);
        sendResponse(exchange, 200, jsonResponse.toString());
    }

    /**
     * Handles GET /api/clifton requests.
     * Returns current parameters of the Clifton.
     */
    private void handleGetClifton(HttpExchange exchange) throws IOException, JSONException {
        log.debug("Received request: GET /api/clifton");
        var json = new JSONObject();

        json.put("windSpeed", cliftonObserver.getWindSpeed());
        json.put("windDirection", cliftonObserver.getWindDirection());
        json.put("latitude", cliftonObserver.getLatitude());
        json.put("longitude", cliftonObserver.getLongitude());
        json.put("velocity", cliftonObserver.getVelocity());
        json.put("heading", cliftonObserver.getHeading());
        json.put("state", cliftonObserver.getState());
        json.put("error", cliftonObserver.isError());
        json.put("waypoint", cliftonObserver.getWaypoint());
        json.put("targetX", cliftonObserver.getTargetX());
        json.put("targetY", cliftonObserver.getTargetY());
        json.put("targetAngle", cliftonObserver.getTargetAngle());
        json.put("targetDistance", cliftonObserver.getTargetDistance());
        json.put("targetRelativeAngle", cliftonObserver.getTargetRelativeAngle());
        json.put("mainSailTargetAngle", cliftonObserver.getMainSailTargetAngle());
        json.put("wingSurface", cliftonObserver.getWingSurface());

        sendResponse(exchange, 200, json.toString());
    }

    /**
     * Handles GET /api/health requests.
     * Returns system health status.
     */
    private void handleGetHealth(HttpExchange exchange) throws IOException, JSONException {
        log.debug("Received request: GET /api/health");
        var json = new JSONObject();

        json.put("status", "ok");

        sendResponse(exchange, 200, json.toString());
    }

    /**
     * Parses request body into JSON object.
     * @param exchange The HttpExchange object
     * @return Parsed JSON object
     */
    private JSONObject parseRequestBody(HttpExchange exchange) throws IOException, JSONException {
        var requestBody = exchange.getRequestBody();
        var requestData = new String(requestBody.readAllBytes(), StandardCharsets.UTF_8);
        return new JSONObject(requestData);
    }

    /**
     * Sends an error response.
     * @param exchange The HttpExchange object
     * @param statusCode HTTP status code
     * @param error Error message
     */
    private void sendError(HttpExchange exchange, int statusCode, String error) throws IOException, JSONException {
        var json = new JSONObject();
        json.put("error", error);
        sendResponse(exchange, statusCode, json.toString());
    }

    /**
     * Sends an HTTP response.
     * @param exchange The HttpExchange object
     * @param statusCode HTTP status code
     * @param response Response as string
     */
    private void sendResponse(HttpExchange exchange, int statusCode, String response) throws IOException {
        exchange.getResponseHeaders().set("Content-Type", "application/json");
        exchange.sendResponseHeaders(statusCode, response.length());
        try (var os = exchange.getResponseBody()) {
            os.write(response.getBytes());
        }
    }
}