package utils;

import java.util.ArrayList;
import java.util.List;

public class Rectangle {

    private List<Vector> points = new ArrayList<>();



    /** Creates a Rectangle with 2 Points and set Tolerance
     *
     */
    public Rectangle(Vector start, Vector end) {
        Vector direction = end.sub(start);
        direction = direction.normalize();

        Vector perpendicularFirst = new Vector(-direction.getY(), direction.getX());
        Vector perpendicularSecond = new Vector(direction.getY(), -direction.getX());

        perpendicularFirst = perpendicularFirst.normalize().scale(Config.PATH_TOLERANCE);
        perpendicularSecond = perpendicularSecond.normalize().scale(Config.PATH_TOLERANCE);

        Vector p1 = start.add(perpendicularFirst);
        Vector p2 = start.add(perpendicularSecond);
        Vector p3 = end.add(perpendicularFirst);
        Vector p4 = end.add(perpendicularSecond);
        points.add(p1);
        points.add(p2);
        points.add(p3);
        points.add(p4);


    }

    public boolean containsPoint(Vector point) {

        // Triangles
        float areaTriangles = getAreaTriangleSum(point);
        // Area Rectangle
        float areaRectangle = getArea();
        //System.out.println("AreaTriangle: " + areaTriangles + "\nAreaRectangle: " + areaRectangle);

        // Comparision
        float epsilon = 0.1f;

        if(Math.abs(areaTriangles - areaRectangle) < epsilon) {
            return true;
        }
        return false;
    }

    public float getAreaTriangleSum(Vector point) {
        float sum = 0;
        // 0,1,p
        sum += (float) calculerAireAvecHeron(points.get(0), points.get(1), point);
        // 0,2,p
        sum += (float) calculerAireAvecHeron(points.get(0), points.get(2), point);
        // 2,3,p
        sum += (float) calculerAireAvecHeron(points.get(2), points.get(3), point);
        // 1,3,p
        sum += (float) calculerAireAvecHeron(points.get(1), points.get(3), point);
        return sum;
    }
    public float getArea() {
        return points.get(0).distanceTo(points.get(1)) * points.get(0).distanceTo(points.get(2));
    }

    public static double calculerAireAvecHeron(Vector p1, Vector p2, Vector p3) {

        float side1 = p1.distanceTo(p2);
        float side2 = p2.distanceTo(p3);
        float side3 = p3.distanceTo(p1);

        // Halbumfang
        double s = (side1 + side2 + side3) / 2;

        return Math.sqrt(s * (s - side1) * (s - side2) * (s - side3));
    }
}
