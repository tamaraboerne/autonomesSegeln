package utils;

public class Point {
    public Vector position;
    public byte status; //

    public Point(Vector position, byte status) {
        this.position = position;
        this.status = status;
    }

    @Override
    public String toString(){
        return position.toString() + " : "+ status;
    }

}
