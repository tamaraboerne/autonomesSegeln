package utils;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class Vector {
    private final float x;
    private final float y;

    public Vector(float x, float y) {
        this.x = x;
        this.y = y;

    }

    public float getX() {
        return x;
    }

    public float getY() {
        return y;
    }

    public Vector add(Vector v) {
        return new Vector(x + v.getX(), y + v.getY());
    }
    public Vector sub(Vector v) {
        return new Vector(x - v.getX(), y - v.getY());
    }



    public Vector scale(float scalar) {
        return new Vector(x * scalar, y * scalar);
    }

    public Vector normalize() {
        float magnitude = magnitude();
        if(magnitude == 0) {
            return new Vector(0, 0);
        }
        return new Vector(x / magnitude,y / magnitude);
    }


    public Vector rotate(float angleDeg) {
        double angleRad = Math.toRadians(angleDeg);
        double cos =  Math.cos(angleRad);
        double sin =  Math.sin(angleRad);

        double newX = this.x * cos - this.y * sin;
        double newY = this.x * sin + this.y * cos;

        return new Vector((float) newX, (float) newY);
    }

    /**
     * Calculates the Euclidean distance between this vector and another vector.
     *
     * @param v the other vector to which the distance is calculated
     * @return the Euclidean distance as a int
     */
    public float distanceTo(Vector v) {
        return (float) Math.sqrt(Math.pow(this.x - v.getX(), 2) + Math.pow(this.y - v.getY(), 2));
    }

    public float distanceToSquared(Vector v) {
        float dx = this.x - v.x;
        float dy = this.y - v.y;
        return dx * dx + dy * dy;
    }


    public float angleDifferenceTo(Vector v) {
        return (float)Math.toDegrees(Math.acos(dotProduct(v) / (magnitude() * v.magnitude())));
    }

    public float angle() {
        //return (float)Math.toDegrees(Math.acos(dotProduct(v) / (magnitude() * v.magnitude()))) * Math.signum(v.getY());
        return (float) ((Math.toDegrees(Math.atan2(y, x)) + 360) % 360);
    }

    public float dotProduct(Vector v) {
        return x * v.getX() + y * v.getY();
    }

    public float magnitude() {
        return distanceTo(new Vector(0, 0));
    }

    public boolean isEqualTo(Vector v) {
        float epsilon = 0.1f;
        return Math.abs(this.x - v.x) < epsilon && Math.abs(this.y - v.y) < epsilon;
    }

    /**
     * Generates a list of random points within specified bounds.
     *
     * @param count Number of points to generate
     * @param xMin Minimum x-value (inclusive)
     * @param xMax Maximum x-value (inclusive)
     * @param yMin Minimum y-value (inclusive)
     * @param yMax Maximum y-value (inclusive)
     * @return List of randomly generated points
     */
    public static List<Vector> generateRandomPoints(int count, int xMin, int xMax, int yMin, int yMax) {
        List<Vector> points = new ArrayList<>(count);
        Random rand = new Random();

        for (int i = 0; i < count; i++) {
            // Generate random coordinates within specified ranges
            int x = xMin + rand.nextInt(xMax - xMin + 1);
            int y = yMin + rand.nextInt(yMax - yMin + 1);
            points.add(new Vector(x, y));
        }
        return points;
    }

    // Berechnet den minimalen Abstand zwischen Punkt und Liniensegment
    /**
     * Berechnet die Projection
     *
     * @param segStart
     * @param segEnd
     * @param point
     * @return
     */
    public static Vector distanceToSegment(Vector segStart, Vector segEnd, Vector point) {

        float segVecX = segEnd.getX() - segStart.getX();
        float segVecY = segEnd.getY() - segStart.getY();

        // Handle case where segStart and segEnd are the same point (segment has zero length)
        float length2 = segVecX * segVecX + segVecY * segVecY;
        if (length2 == 0) {
            return new Vector(segStart.getX(), segStart.getY()); // Projection is just segStart
        }

        float pointVecX = point.getX() - segStart.getX();
        float pointVecY = point.getY() - segStart.getY();

        // Compute t = (AB · AC) / ||AB||²
        float t = (pointVecX * segVecX + pointVecY * segVecY) / length2;

        // Clamp t to [0, 1] to ensure projection lies on the segment
        t = Math.max(0, Math.min(1, t));

        // Compute projection point H = A + t * AB
        float projX = segStart.getX() + t * segVecX;
        float projY = segStart.getY() + t * segVecY;

        return new Vector(projX, projY);
    }

    public String toString() {
        return "(" + this.x + ", " + this.y + ")";
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Vector) {
            return isEqualTo((Vector) obj);
        }
        return false;

    }

    @Override
    public int hashCode() {
        return (int)(x * 10000 + y);
    }
}
