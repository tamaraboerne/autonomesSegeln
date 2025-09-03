package utils;

public class Circle {

    private Vector position;
    private float radius;

    public Circle(Vector position, float radius) {
        this.position = position;
        this.radius = radius;
    }

    public boolean containsPoint(Vector point) {
        //System.out.println("Distanz: " + position.distanceTo(point) + "Radius: " + radius);
        return position.distanceTo(point) <= radius;
    }
}
