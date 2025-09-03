package routeAlgorithm;

import utils.Config;

public class Node {

    public State state;
    public Node parentNode;
    float pathCosts;

    public Node(State myState, Node parentNode) {
        this.state = myState;
        this.parentNode = parentNode;
        if (parentNode != null){
            this.pathCosts = parentNode.pathCosts + parentNode.state.position.distanceTo(myState.position) + Config.PATH_NODE_PENALTY;
        }
    }

    public int depth(){
        return 1 + (parentNode != null ? parentNode.depth() : 0);
    }


    public boolean isPositionEqualTo(Node n) {
        return this.state.position.equals(n.state.position);
    }

    @Override
    public String toString(){
        return (parentNode != null ? parentNode.toString() : "null") + " -> " + "[" + state.toString() + ", " + pathCosts + "]";
    }
}
