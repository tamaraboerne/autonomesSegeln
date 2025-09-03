package utils;

import java.util.List;

public class Config {


    public static final int VISUALIZER_SCREENSIZE_X = 800;
    public static final int VISUALIZER_SCREENSIZE_Y = 800;

    public static float MIN_ANGLE_OF_ATTACK = 45; // TODO
    public static float MAX_ANGLE_OF_ATTACK = 135;

    public static final float TURNING_RADIUS = 160;

    public static final float MIN_WIND_SPEED = 2;

    public static final float PATH_TOLERANCE =  100;

    public static final String grid = "static"; // "static" or "dynamic"
    public static int GRID_CELL_SIZE = 200;
    public static final int DYNAMIC_STEP_SIZE = 100;

    public static double ORIGIN_LAT = 53.8345;
    public static double ORIGIN_LON = 10.69953;

    public static float PATH_NODE_PENALTY = 1;

    // Points A-G as GPS in decimal degrees:
    public static List<Tuple<Double>> GPS_POINT = List.of(
            new Tuple<Double>(53.83447994, 10.69952499), // A
            new Tuple<Double>(53.83479481, 10.70054092), // B
            new Tuple<Double>(53.83472304, 10.70060538), // C
            new Tuple<Double>(53.83432977, 10.69942065), // D
            new Tuple<Double>(53.83509392, 10.69868656), // E
            new Tuple<Double>(53.83539367, 10.69954897), // F
            new Tuple<Double>(53.83521470, 10.69969939),  // G
            new Tuple<Double>(53.83498565, 10.69901467)
    );

    public static int SERVER_API_PORT = 8080;
}
