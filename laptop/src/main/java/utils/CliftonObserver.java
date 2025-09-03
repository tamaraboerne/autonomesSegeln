package utils;

import model.SensorData;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import polygon.Probe.CoordinateSystem;

import java.util.Observable;
import java.util.Observer;

public class CliftonObserver implements Observer {

    private static final Logger log = LogManager.getLogger(CliftonObserver.class);

    private boolean debug = false;
    private double windSpeed;
    private short windDirection;
    private float latitude;
    private float longitude;
    private float velocity;
    private short heading;
    private int state;
    private byte waypoint;
    private boolean error;
    private short targetX;
    private short targetY;
    private float targetAngle;
    private float targetRelativeAngle;
    private short targetDistance;
    private float mainSailTargetAngle;
    private short wingSurface;
    private static CliftonObserver instance;

    private CliftonObserver() {}

    public static CliftonObserver clifton() {
        if (instance == null) {
            instance = new CliftonObserver();
        }
        return instance;
    }

    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof SensorData) {
            // Updating Observer Parameters based on observables data
            var x = (float) ((SensorData) arg).x();
            var y = (float) ((SensorData) arg).y();
            setLocalCoordinates(x, y);
            this.velocity = ((SensorData) arg).velocity();
            this.heading = ((SensorData) arg).heading();
            this.state = ((SensorData) arg).stateClifton().getCode();
            this.error = ((SensorData) arg).error();
            this.waypoint = ((SensorData) arg).waypoint();
            this.targetX = ((SensorData) arg).targetX();
            this.targetY = ((SensorData) arg).target_y();
            this.targetAngle = ((SensorData) arg).targetAngle();
            this.targetRelativeAngle = ((SensorData) arg).targetRelativeAngle();
            this.targetDistance = ((SensorData) arg).targetDistance();
            this.mainSailTargetAngle = ((SensorData) arg).mainSailTargetAngle();
            this.wingSurface = ((SensorData) arg).wingSurface();

            // if debug is enabled, we do not auto-update wind speed and direction
            if (!debug) {
                this.windSpeed = ((SensorData) arg).windSpeed();
                this.windDirection = ((SensorData) arg).windDirection();
            }
            log.debug("Windspeed={}, Latitude={}, Longitude={}, Velocity={}, Heading={}", windSpeed, latitude, longitude, velocity, heading);
        }
    }

    public void updateWindSpeed(double windSpeed) {
        this.windSpeed = windSpeed;
    }

    public void updateCoordinates(double x, double y) {
        this.latitude = (float) x;
        this.longitude = (float) y;
    }

    public void setDebug(boolean debug) {
        this.debug = debug;
    }

    public void updateWindDirection(short windDirection) {
        this.windDirection = windDirection;
    }

    public void updateHeading(short heading) {
        this.heading = heading;
    }

    public boolean getDebug() {
        return debug;
    }

    public double getWindSpeed() {
        return windSpeed;
    }

    public float getLatitude() {
        return latitude;
    }

    public float getLongitude() {
        return longitude;
    }

    public float getVelocity() {
        return velocity;
    }

    public short getHeading() {
        return heading;
    }

    public short getWindDirection() {
        return windDirection;
    }

    public int getState() {
        return state;
    }

    public byte getWaypoint() {
        return waypoint;
    }

    public boolean isError() {
        return error;
    }

    public short getTargetX() {
        return targetX;
    }

    public short getTargetY() {
        return targetY;
    }

    public float getTargetAngle() {
        return targetAngle;
    }

    public float getTargetRelativeAngle() {
        return targetRelativeAngle;
    }

    public short getTargetDistance() {
        return targetDistance;
    }

    public float getMainSailTargetAngle() {
        return mainSailTargetAngle;
    }

    public short getWingSurface() {
        return wingSurface;
    }

    public void setLocalCoordinates(float x, float y) {
        CoordinateSystem cs = CoordinateSystem.coordinateSystem();
        Vector local = new Vector(x, y);
        Tuple<Double> gps = cs.localVectorToGPS(local);
        this.latitude = gps.first.floatValue();
        this.longitude = gps.second.floatValue();
    }
}