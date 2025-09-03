package utils;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.stage.Stage;
import polygon.Probe.convexe.Polygon;
import java.util.ArrayList;
import java.util.List;

public class PathfindingVisualizer extends Application{


    public static Color expandColor = Color.PURPLE;
    public static final Pane root = new Pane();

    @Override
    public void start(Stage primaryStage) throws Exception {
        Scene scene = new Scene(root, Config.VISUALIZER_SCREENSIZE_X, Config.VISUALIZER_SCREENSIZE_Y);

        primaryStage.setTitle("Pathfinding Visualizer");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void begin() {
        launch();
    }

    /** Draws Initial Information to the Screen (grid, start, end, Wind-Direction and Sailing Area)
     *
     * @param start Startposition of the Cliftion
     * @param end Destination of the Clifton
     * @param nodes Nodes Inside the Sailing Area
     * @param windAngle Angle of the Wind
     */
    public static void initAlgorithm(Vector start, Vector end, Vector[] nodes, float windAngle) {
        // Draw Nodes
        for(Vector node: nodes) {
            drawNode(node, "grid");
        }
        // Draw Start/ Endnode
        drawNode(start, "start");
        drawNode(end, "end");
        drawWindAngle(windAngle);
        drawArea();
    }

    /** Draws the Sailing Area on the Screen
     *
     */
    public static void drawArea() {
        List<Vector> area = Polygon.sailingArea().getPoints();
        //System.out.println("Area Points: " + Polygon.sailingArea().getPoints());
        Vector previous = area.get(0);
        for(int i = 0; i < area.size(); i++) {
            drawNode(area.get(i), "area");
        }
        for(int i = 1; i < area.size(); i++) {
            Vector point = area.get(i);
            Vector screenPrevious = worldToScreen(previous);
            Vector screenCurrent = worldToScreen(point);
            Line line = new Line(screenPrevious.getX(), screenPrevious.getY(), screenCurrent.getX(), screenCurrent.getY());
            line.setStroke(Color.DEEPPINK);
            line.setStrokeWidth(2);
            root.getChildren().add(line);
            previous = area.get(i);
        }
        Vector screenFirst = worldToScreen(area.getFirst());
        Vector screenLast = worldToScreen(area.getLast());
        Line line = new Line(screenFirst.getX(), screenFirst.getY(), screenLast.getX(), screenLast.getY());
        line.setStroke(Color.DEEPPINK);
        line.setStrokeWidth(2);
        root.getChildren().add(line);
    }

    /** Draws Node in different colors and sizes based on the given type
     *
     * @param worldPosition Position of the Node
     * @param type Type of the Node, to distinguish between different types of Points on the Screen
     */
    public static void drawNode(Vector worldPosition, String type) {
        Vector screenCoords = worldToScreen(worldPosition);
        Circle circle = null;
        switch(type) {
            case "grid":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 1);
                circle.setFill(Color.DODGERBLUE);
                break;
            case "expand":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 2);
                circle.setFill(expandColor);
                expandColor = Color.hsb(expandColor.getHue() + 0.1, expandColor.getSaturation(), expandColor.getBrightness());
                break;
            case "start":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 7);
                circle.setFill(Color.LIGHTGREEN);
                break;
            case "end":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 7);
                circle.setFill(Color.RED);
                break;
            case "path":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 4);
                circle.setFill(Color.ORANGE);
                break;
            case "valid":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 3);
                circle.setFill(Color.YELLOW);
                break;
            case "invalid":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 3);
                circle.setFill(Color.RED);
                break;
            case "area":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 4);
                circle.setFill(Color.DEEPPINK);
                break;
            case "curve":
                circle = new Circle(screenCoords.getX(), screenCoords.getY(), 5);
                circle.setFill(Color.DEEPSKYBLUE);
        }
        root.getChildren().add(circle);
    }

    /** Calculates where the Position should be drawn on the Screen
     *
     * @param worldPosition Position to be converted
     * @return Coordinates on the Screen
     */
    public static Vector worldToScreen(Vector worldPosition) {
        float normX = (float) (worldPosition.getX() - Polygon.sailingArea().minX()) / (Polygon.sailingArea().maxX() - Polygon.sailingArea().minX());
        float normY =  1f  - (float) (worldPosition.getY() - Polygon.sailingArea().minY()) / (Polygon.sailingArea().maxY() - Polygon.sailingArea().minY());

        int screenX = Math.round(Config.VISUALIZER_SCREENSIZE_X * normX);
        int screenY = Math.round(Config.VISUALIZER_SCREENSIZE_Y * normY);

        return new Vector(screenX, screenY);
    }

    /** Draws the Path to the Screen, using Circles for every Point and Lines to connect consecutive Points
     *
     * @param path Array of Points containing the calculated, unsmoothed Path
     */
    public static void drawPath(Point[] path) {
        Vector previous = path[0].position;
        Vector screenPrev;
        Vector screenCurr;
        drawNode(previous, "path");
        for (int i = 1; i < path.length; i++) {
            drawNode(path[i].position, "path");
            screenPrev = worldToScreen(previous);
            screenCurr = worldToScreen(path[i].position);
            Line line = new Line(screenPrev.getX(), screenPrev.getY(), screenCurr.getX(), screenCurr.getY());
            line.setStroke(Color.RED);
            line.setStrokeWidth(2);
            root.getChildren().add(line);
            previous = path[i].position;
        }
    }

    /** Draws Every Curve-Point onto the Screen
     *
     * @param path Array of Points containing the smoothed Path
     */
    public static void drawSmoothPath(Point[] path) {
        //System.out.println(Arrays.toString(path));
        if(path.length == 0) {
            System.out.println("Path is empty");
            return;
        }
        for(int i = 1; i < path.length - 1; i++) {
            drawNode(path[i].position, "curve");
        }
    }

    /** Draws an Arrow indicating the Wind-direction on the Screen
     *
     * @param windAngle
     */
    public static void drawWindAngle(float windAngle) {
        double radians = Math.toRadians(windAngle);

        float arrowLength = 100;
        int windDx = (int) Math.round(Math.cos(radians) * arrowLength);
        int windDy = (int) Math.round(-Math.sin(radians) * arrowLength);

        Vector arrowStart = new Vector(700, 100);
        Vector windDirection = new Vector(windDx, windDy);
        Vector arrowEnd = arrowStart.add(windDirection.scale(-1));

        ArrayList<Line> lines = new ArrayList<>();
        lines.add(new Line(arrowStart.getX(), arrowStart.getY(), arrowEnd.getX(), arrowEnd.getY()));

        // Arrowhead
        double len = Math.sqrt(windDx * windDx + windDy * windDy);

        double arrowHeadLength = 30;
        double dirX = (windDx / len) * arrowHeadLength;
        double dirY = (windDy / len) * arrowHeadLength;

        Vector arrowHead1 = new Vector((int) dirX, (int) dirY).rotate(30);
        Vector arrowHead2 = new Vector((int) dirX, (int) dirY).rotate(-30);

        Vector arrowHead1End = arrowEnd.add(new Vector(arrowHead1.getX(), arrowHead1.getY()));
        Vector arrowHead2End = arrowEnd.add(new Vector(arrowHead2.getX(), arrowHead2.getY()));

        lines.add(new Line(arrowEnd.getX(), arrowEnd.getY(), arrowHead1End.getX(), arrowHead1End.getY()));
        lines.add(new Line(arrowEnd.getX(), arrowEnd.getY(), arrowHead2End.getX(), arrowHead2End.getY()));

        for(Line line : lines) {
            line.setStroke(Color.ORANGE);
            line.setStrokeWidth(7);
        }
        root.getChildren().addAll(lines);
    }


}
