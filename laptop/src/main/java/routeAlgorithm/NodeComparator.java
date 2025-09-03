package routeAlgorithm;


import utils.Vector;
import java.util.Comparator;


public class NodeComparator implements Comparator<Node> {
    private Vector destination;
    private float windAngle;

    public NodeComparator(Vector destination, float windAngle) {
        this.destination = destination;
    }


    @Override
    public int compare(Node o1, Node o2) {
        // If the f value of both nodes is the same, return the compare result for their hashCodes instead.
        // (Required to have different nodes of the same f value in a TreeSet)
        if(f(o1) == f(o2))
            return Integer.compare(o1.hashCode(), o2.hashCode());
        return Float.compare(f(o1), f(o2));
    }

    public float f(Node node) {
        //Vector direction = new Vector(destination.getX() - node.state.position.getX(), destination.getY() - node.state.position.getY());
        //float angle = direction.angle();
        //boolean validToWind = AStar.validToWind(angle, windAngle);

        return node.pathCosts + node.state.position.distanceTo(destination);
        //return node.pathCosts + node.state.position.distanceTo(destination) * (validToWind ? 1f : 1.1f);
        //return node.pathCosts + node.state.position.distanceTo(destination) * (AStar.isCandidateValid(node.state.position, destination) ? 1f : 1.2f);

    }
}
