package polygon.Probe.convexe;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import polygon.Probe.CoordinateSystem;
import utils.Config;
import utils.Vector;

import java.util.ArrayList;
import java.util.List;

public class Polygon {

    private static final Logger log = LogManager.getLogger(Polygon.class);

    private final List<Vector> points;

    public static Polygon sailingArea;

    // Returns the singleton instance of the sailing area polygon.
    public static Polygon sailingArea() {
        if (sailingArea == null) {
            sailingArea = new Polygon(
                    CoordinateSystem.coordinateSystem().convertGPSToVectors(Config.GPS_POINT)
            );
        }
        return sailingArea;
    }

    /**
     * Constructor
     * @param vertices List of polygon vertices (minimum 3)
     */
    public Polygon(List<Vector> vertices) {
        if (vertices.size() < 3) {
            throw new IllegalArgumentException("A polygon must have at least 3 vertices.");
        }
        this.points = new ArrayList<>(vertices);
    }

    public List<Vector> getPoints() {
        return points;
    }


    /**
     * Checks whether a given point is inside the polygon.
     * @param testPoint The point to check
     * @return true if the point lies within the polygon
     */
    public boolean containsPoint(Vector testPoint) {
        boolean inside = false;
        int n = points.size();

        for (int i = 0, j = n - 1; i < n; j = i++) {
            Vector p1 = points.get(i);
            Vector p2 = points.get(j);

            if (((p1.getY() > testPoint.getY()) != (p2.getY() > testPoint.getY())) &&
                    (testPoint.getX() < (p2.getX() - p1.getX()) * (testPoint.getY() - p1.getY()) / (p2.getY() - p1.getY()) + p1.getX())) {
                inside = !inside;
            }
        }
        return inside;
    }

    /**
     * Checks if a target position is valid, i.e., reachable from the current position
     * without leaving the polygon.
     * @param currentPosition Current position
     * @param candidatePosition Target position
     * @param territory The polygon representing the valid territory
     * @return true if the target is within the polygon and the path does not exit it
     */
    public static boolean isPointValid(Vector currentPosition, Vector candidatePosition, Polygon territory) {
        return territory.containsPoint(candidatePosition) &&
                territory.isPathInTerritory(currentPosition, candidatePosition);
    }

    /**
     * Checks if the straight path between two points lies entirely within the polygon.
     * @param start Start vector
     * @param end End vector
     * @return true if the path is fully within the polygon
     */
    public boolean isPathInTerritory(Vector start, Vector end) {
        // Check multiple points along the line between start and end
        final int SAMPLES = 500; // More samples = more precise

        for (int i = 0; i <= SAMPLES; i++) {
            float t = (float)i / SAMPLES;
            float x = start.getX() + t * (end.getX() - start.getX());
            float y = start.getY() + t * (end.getY() - start.getY());

            if (!containsPoint(new Vector(x, y))) return false;
        }

        return true;
    }

    /**
     * Checks if the segment from A to B crosses or exits the polygon boundary.
     * @param A Start point
     * @param B End point
     * @return true if the segment exits the polygon
     */
    public static boolean segmentLeavesPolygon(Vector A, Vector B) {

        // Check if either point is outside the polygon
        if (!sailingArea.containsPoint(A) || !sailingArea.containsPoint(B)) return true;

        List<Vector> vertices = sailingArea.getPoints();
        int n = vertices.size();

        // Check all polygon edges
        for (int i = 0; i < n; i++) {
            Vector C = vertices.get(i);
            Vector D = vertices.get((i + 1) % n);

            // Skip edges directly connected to A or B
            if (C.equals(A) || C.equals(B) || D.equals(A) || D.equals(B)) continue;

            if (segmentsIntersect(A, B, C, D)) return true;
        }
        return false;
    }


    /**
     * Checks whether a point lies near a segment within a tolerance radius.
     * @param segmentStart Start of the segment
     * @param segmentEnd End of the segment
     * @param testPoint The point to test
     * @param territory The polygon territory
     * @param toleranceRadius Allowed distance to the segment
     * @return true if the point is within tolerance distance of the segment
     */
    public static boolean isPointNearSegmentValid(Vector segmentStart, Vector segmentEnd, Vector testPoint, Polygon territory, float toleranceRadius) {

        if (segmentStart == null || segmentEnd == null || testPoint == null || territory == null) {
            log.warn("Null parameter in segment proximity check: segStart={}, segEnd={}, testPoint={},  territory={}",
                    segmentStart, segmentEnd, testPoint, territory);
            return false;
        }

        // Compute the closest point on the segment
        Vector projection = getPointOnSegment(segmentStart, segmentEnd, testPoint);
        log.debug("Calculated nearest point on segment: segStart={}, segEnd={}, testPoint={}: {}",
                segmentStart, segmentEnd, testPoint, projection);

        return distance(projection, testPoint) <= toleranceRadius;
    }

    /**
     * Calculates Euclidean distance between two points.
     */
    private static double distance(Vector p, Vector q) {
        return Math.sqrt((q.getX() - p.getX()) * (p.getY() - q.getY()));
    }

    /**
     * Finds the closest point on a segment to a given external point.
     * @param A Start of the segment
     * @param B End of the segment
     * @param P The point to project
     * @return The projected point on the segment
     */
    private static Vector getPointOnSegment(Vector A, Vector B, Vector P) {
        float abx = B.getX() - A.getX();                 // AB
        float aby = B.getY() - A.getY();
        float apx = P.getX() - A.getX();                 // AP
        float apy = P.getY() - A.getY();

        // Skalarprodukt von AM und AB berechnen
        float ab_ap = abx * apx + aby * apy;

        // Länge von AB im Quadrat
        float ab2 = abx * abx + aby * aby;

        // Parameter t berechnen (Anteil der Projektion auf AB)
        float t = ab_ap / ab2;

        // Wenn t < 0, liegt die Projektion vor A
        // Wenn t > 1, liegt die Projektion hinter B
        // Ansonsten liegt sie zwischen A und B
        t = Math.max(0, Math.min(1, t));

        // Projektion H berechnen
        float Hx = A.getX() + abx * t;
        float Hy = A.getY() + aby * t;

        return new Vector(Hx, Hy);
    }

    public float minX (){
        float curMin = Float.MAX_VALUE;
        for (Vector v : points) {
            curMin = Math.min(curMin, v.getX());
        }
        return curMin;
    }


    public float minY (){
        float curMin = Float.MAX_VALUE;
        for (Vector v : points) {
            curMin = Math.min(curMin, v.getY());
        }
        return curMin;
    }

    public float maxX (){
        float curMax = Float.MIN_VALUE;
        for (Vector v : points) {
            curMax = Math.max(curMax, v.getX());
        }
        return curMax;
    }

    public float maxY (){
        float curMax = Float.MIN_VALUE;
        for (Vector v : points) {
            curMax = Math.max(curMax, v.getY());
        }
        return curMax;
    }

    // Determines if two segments intersect
    private static boolean segmentsIntersect(Vector a1, Vector a2, Vector b1, Vector b2) {

        // Berechne die Orientierungen für alle möglichen Kombinationen
        int o1 = orientation(a1, a2, b1);
        int o2 = orientation(a1, a2, b2);
        int o3 = orientation(b1, b2, a1);
        int o4 = orientation(b1, b2, a2);

        return (o1 != o2 && o3 != o4) ||
                (o1 == 0 && onSegment(a1, b1, a2)) ||
                (o2 == 0 && onSegment(a1, b2, a2)) ||
                (o3 == 0 && onSegment(b1, a1, b2)) ||
                (o4 == 0 && onSegment(b1, a2, b2));
    }

    /**
     * Determines the orientation of three points (clockwise, counterclockwise, or collinear).
     */
    private static int orientation(Vector p, Vector q, Vector r) {
        float val = (q.getY() - p.getY()) * (r.getX() - q.getX())
                - (q.getX() - p.getX()) * (r.getY() - q.getY());

        if (Math.abs(val) < 1e-10) return 0;    // Kollinear mit Toleranz
        return (val > 0) ? 1 : 2;               // Clockwise or counter-clockwise
    }

    /**
     * Checks whether point q lies on the segment pr.
     */
    private static boolean onSegment(Vector p, Vector q, Vector r) {
        return q.getX() >= Math.min(p.getX(), r.getX()) &&
                q.getX() <= Math.max(p.getX(), r.getX()) &&
                q.getY() >= Math.min(p.getY(), r.getY()) &&
                q.getY() <= Math.max(p.getY(), r.getY());
    }

}