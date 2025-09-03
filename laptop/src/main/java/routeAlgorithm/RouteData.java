package routeAlgorithm;

import model.SensorData;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import utils.Vector;

import java.util.LinkedList;
import java.util.Queue;
import java.util.Observable;
import java.util.Observer;

public class RouteData implements Observer {

    private static final Logger log = LogManager.getLogger(RouteData.class);

    public Vector currentPosition;

    public float orientationClifton;

    static final int WINDOW_SIZE = 100;

    Queue<Float> windDirectionWindow = new LinkedList<>();
    Queue<Float> windSpeedWindow = new LinkedList<>();

    private static RouteData instance;


    public static RouteData routeData() {
        if (instance == null) {
            instance = new RouteData();
        }
        return instance;
    }

    /**
     *
     * @return the moving average of the wind speed
     */
    public float getWindSpeed(){
        while(windSpeedWindow.size() < WINDOW_SIZE){
            try {
                log.info("Waiting for sufficient wind speed data: {}/{}", + windSpeedWindow.size(), WINDOW_SIZE);
                Thread.sleep(500);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        float windSpeed = 0;
        for(float speed : windSpeedWindow){
            windSpeed += speed;
        }
        return windSpeed / windSpeedWindow.size();
    }

    /**Get Wind Direction relative to the Clifton
     *
     * @return the moving average of the wind direction
     */
    public float getWindDirection() {
        Vector windDirection = new Vector(0, 0);

        while(windDirectionWindow.size() < WINDOW_SIZE) {
            try {
                log.info("Waiting for sufficient Wind Direction data : {}/{}", windDirectionWindow.size(), WINDOW_SIZE);
                Thread.sleep(500);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        for(float dir : windDirectionWindow){
            windDirection = windDirection.add(angleToVector(dir));
        }
        return  windDirection.angle();
    }


    /**
     *
     * @param o     the observable object.
     * @param arg   an argument passed to the {@code notifyObservers}
     *                 method.
     */
    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof SensorData) {
            currentPosition = new Vector(((SensorData) arg).x(), ((SensorData) arg).y());
            this.orientationClifton = ((SensorData) arg).heading();

            windSpeedWindow.add(((SensorData) arg).windSpeed());

            float windDir = ((SensorData) arg).windDirection();
            windDir = (windDir + orientationClifton) % 360; // Convert wind direction in relation to the clifton to absolute
            windDirectionWindow.add(windDir);

            if(windSpeedWindow.size() > WINDOW_SIZE){
                windSpeedWindow.remove();
            }

            if(windDirectionWindow.size() > WINDOW_SIZE) {
                windDirectionWindow.remove();
            }
            //this.windSpeed = ((SensorData) arg).windSpeed();
            //this.windDirection = ((SensorData) arg).windDirection();
        }
    }


    /**
     *
     * @param angle to be converted to a vector
     * @return normalized Vector pointing in the direction of the angle
     */
    public static Vector angleToVector(float angle){
        return new Vector((float)Math.cos(Math.toRadians(angle)), (float)Math.sin(Math.toRadians(angle)));
    }


    /**
     *
     * @param angle in degrees with 0° north increasing clockwise
     * @return angle in degrees with 0° east increasing counter-clockwise
     */
    public static float convertAngle(float angle) {
        return ((angle * -1) + 90 + 360) % 360;
    }

}
