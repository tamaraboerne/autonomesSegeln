package polygon.Probe.convexe;

import utils.Vector;

import java.util.ArrayList;
import java.util.List;

import static polygon.Probe.convexe.Polygon.isPointNearSegmentValid;
import static polygon.Probe.convexe.Polygon.isPointValid;


public class Concave_ohne_schutztreifen {


    public static void main(String[] args) {
        
        // Define polygon points
        Vector A = new Vector(-25.0f, 50.0f);
        Vector B = new Vector(35.0f, 50.0f);
        Vector C = new Vector(35.0f, 35.0f);
        Vector D = new Vector(35.0f, 10.0f);
        Vector E = new Vector(5.0f, 10.0f);
        Vector F = new Vector(60.0f, 5.0f);
        Vector G = new Vector(60.0f, -15.0f);
        Vector H = new Vector(-15.0f, -25.0f);

        

        // Create polygon
        List<Vector> points = new ArrayList<>();
        points.add(A);
        points.add(B);
        points.add(C);
        points.add(D);
        points.add(E);
        points.add(F);
        points.add(G);
        points.add(H);


        
        // Initialize polygon objects
        Polygon polygon1 = new Polygon(points);

        // Generate 10,000 random test points within the coordinate range
        int numPoints = 10000;
        List<Vector> randomPoints = Vector.generateRandomPoints(
                numPoints,
                -30, 70,   // x-range (min, max)
                -30, 60    // y-range (min, max)
        );

        // Start performance timer
        long startTime = System.nanoTime();

        // Counters for points inside each polygon
        int countPoly = 0;

        // Test each random point against all polygons
        for (Vector p : randomPoints) {
            if (polygon1.containsPoint(p)) countPoly++;
        }
        Vector teta = new Vector(-5.0f, 45.0f);
        Vector delta = new Vector(-20.0f, 25.0f);

        Vector u = new Vector(5.0f, 25.0f);
        Vector v = new Vector(30.0f, -10.0f);
        System.out.println("intervalle [uv] " + isPointValid(u, v,polygon1));
        //System.out.println("intervalle [uv] " + segmentVerlaesstPolygon(u, v));

    //  test der tolerance
        Vector testPoint1 = new Vector(-10.0f, 45.0f);
        Vector testPoint2 = new Vector(-15.0f, 35.0f);
        Vector testPoint3 = new Vector(-17.5f, 25.0f);
        System.out.println("intervalle [delta-teta] " + isPointNearSegmentValid(delta, teta, testPoint1, polygon1, 5.0f));
        System.out.println("intervalle [delta-teta] " + isPointNearSegmentValid(delta, teta, testPoint2, polygon1, 5.0f));
        System.out.println("intervalle [delta-teta] " + isPointNearSegmentValid(delta, teta, testPoint3, polygon1, 5.0f));


        // Stop timer and calculate duration
        long endTime = System.nanoTime();
        double duration = (endTime - startTime) / 1_000_000.0; // Convert to milliseconds

        // Print test results
        System.out.println("Results after testing " + numPoints + " points:");
        System.out.println("Points in Polygon: " + countPoly);
        System.out.printf("Total time: %.3f ms\n", duration);
        System.out.printf("Average time per point: %.6f ms\n", duration/numPoints);
    }
}
