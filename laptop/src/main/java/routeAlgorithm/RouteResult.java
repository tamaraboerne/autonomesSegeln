package routeAlgorithm;

import utils.Tuple;

public class RouteResult {
    public final Tuple<Double>[] route;
    public final boolean success;
    public final String message;

    public RouteResult(Tuple<Double>[] route, boolean success, String message) {
        this.route = route;
        this.success = success;
        this.message = message;
    }
}


