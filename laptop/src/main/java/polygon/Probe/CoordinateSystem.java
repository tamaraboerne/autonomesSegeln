package polygon.Probe;

import utils.Config;
import utils.Tuple;
import utils.Vector;
import java.util.Arrays;
import java.util.List;

public class CoordinateSystem {

    private static final double EARTH_RADIUS = 6371000.0; // Erdradius in Metern
    private final double lat0; // Ursprung Breitengrad in Dezimalgrad
    private final double lon0; // Ursprung Längengrad in Dezimalgrad

    private static CoordinateSystem instance;


    public static CoordinateSystem coordinateSystem() {
        if (instance == null) {
            instance = new CoordinateSystem(Config.ORIGIN_LAT, Config.ORIGIN_LON);
        }
        return instance;
    }

    /**
     * Konstruktor mit festem GPS-Ursprung (aus dem Revier-Polygon)
     * @param lat0 Breitengrad des Ursprungs in Dezimalgrad
     * @param lon0 Längengrad des Ursprungs in Dezimalgrad
     */
    private CoordinateSystem(double lat0, double lon0) {
        this.lat0 = lat0;
        this.lon0 = lon0;
    }

    /**
     * Rechnet GPS-Koordinaten (lat/lon) relativ zum Ursprung in X/Y-Zentimeter um
     * Y zeigt nach Norden, X zeigt nach Osten
     */
    public Vector gpsToLocalVector(Tuple<Double> gps) {
        double dLat = Math.toRadians(gps.first - lat0);
        double dLon = Math.toRadians(gps.second - lon0);
        double meanLat = Math.toRadians((gps.first + lat0) / 2.0);

        double x = dLon * Math.cos(meanLat) * EARTH_RADIUS;
        double y = dLat * EARTH_RADIUS;

        return new Vector((float) (x * 100), (float) (y * 100)); // Zentimeter
    }

    /**
     * Rechnet einen lokalen Vektor in GPS-Koordinaten zurück
     * @param local lokaler Vektor in Zentimeter
     * @return Array [lat, lon] in Dezimalgrad
     */
    public Tuple<Double> localVectorToGPS(Vector local) {
        double x = local.getX() / 100.0;
        double y = local.getY() / 100.0;

        double dLat = y / EARTH_RADIUS;
        double dLon = x / (EARTH_RADIUS * Math.cos(Math.toRadians(lat0)));

        double lat = lat0 + Math.toDegrees(dLat);
        double lon = lon0 + Math.toDegrees(dLon);

        return new Tuple<>(lat, lon);
    }

    /**
     * Gibt die GPS-Koordinaten des Ursprungs zurück
     */
    public double[] getOriginGPS() {
        return new double[] { lat0, lon0 };
    }



    public List<Vector> convertGPSToVectors(List<Tuple<Double>> gps) {

        // Umrechnung in lokale Vektoren:
        Vector[] localVectors = new Vector[gps.size()];
        for (int i = 0; i < gps.size(); i++) {
            localVectors[i] = gpsToLocalVector(gps.get(i));
        }

        return Arrays.asList(localVectors);
    }
}

