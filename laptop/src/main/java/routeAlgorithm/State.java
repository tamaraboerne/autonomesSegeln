package routeAlgorithm;

import utils.Vector;

public class State {
    public final Vector position;
    public final float directionAngle; // In degrees

    public State(Vector position, float directionAngle) {
        this.position = position;
        this.directionAngle = directionAngle;
    }

    public boolean isPositionEqualTo(State s) {
        return position.isEqualTo(s.position);
    }

    public boolean isEqualTo(State s) {
        float epsilon = 0.0001f;
        return this.position.isEqualTo(s.position) && Math.abs(this.directionAngle - s.directionAngle) > epsilon;
        //return this.position.isEqualTo(s.position);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof State) {
            return isEqualTo((State) obj);
        }
        return false;
    }

    @Override
    public int hashCode() {
        return (int) (this.position.hashCode() * 10000 + directionAngle);
    }

    public String toString(){
        return position.getX() + "/" + position.getY() + " " + directionAngle + "°";
    }
}
