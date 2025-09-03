package polygon.Probe.convexe;

import utils.Vector;

import java.util.ArrayList;
import java.util.List;

import static utils.Vector.distanceToSegment;

public class Convexe_ohne_visual {
    public static void main(String[] args) {
        // Define polygon points
        Vector A = new Vector(-25.0f, 50.0f);
        Vector B = new Vector(35.0f, 50.0f);
        Vector C = new Vector(35.0f, 35.0f);
        Vector D = new Vector(10.0f, 35.0f);
        Vector E = new Vector(10.0f, 5.0f);
        Vector F = new Vector(60.0f, 5.0f);
        Vector G = new Vector(60.0f, -15.0f);
        Vector H = new Vector(-25.0f, -15.0f);
        Vector I = distanceToSegment(A, H, E);
        Vector J = distanceToSegment(A, H, D);

        // Create polygon 1 (top section)
        List<Vector> hochpoints = new ArrayList<>();
        hochpoints.add(A);
        hochpoints.add(B);
        hochpoints.add(C);
        hochpoints.add(J);

        // Create polygon 2 (middle section)
        List<Vector> mittepoints = new ArrayList<>();
        mittepoints.add(E);
        mittepoints.add(D);
        mittepoints.add(I);
        mittepoints.add(J);

        // Create polygon 3 (bottom section)
        List<Vector> unterepoints = new ArrayList<>();
        unterepoints.add(I);
        unterepoints.add(F);
        unterepoints.add(G);
        unterepoints.add(H);

        // Initialize polygon objects
        Polygon polygon1 = new Polygon(hochpoints);
        Polygon polygon2 = new Polygon(mittepoints);
        Polygon polygon3 = new Polygon(unterepoints);

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
        int countPoly1 = 0, countPoly2 = 0, countPoly3 = 0;

        // Test each random point against all polygons
        for (Vector p : randomPoints) {
            if (polygon1.containsPoint(p)) countPoly1++;
            if (polygon2.containsPoint(p)) countPoly2++;
            if (polygon3.containsPoint(p)) countPoly3++;
        }



        // Stop timer and calculate duration
        long endTime = System.nanoTime();
        double duration = (endTime - startTime) / 1_000_000.0; // Convert to milliseconds

        // Print test results
        System.out.println("Results after testing " + numPoints + " points:");
        System.out.println("Points in Polygon1: " + countPoly1);
        System.out.println("Points in Polygon2: " + countPoly2);
        System.out.println("Points in Polygon3: " + countPoly3);
        System.out.printf("Total time: %.3f ms\n", duration);
        System.out.printf("Average time per point: %.6f ms\n", duration/numPoints);
    }
}