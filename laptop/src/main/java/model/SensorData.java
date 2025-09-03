package model;

// todo: status -> split into multiple fields
public record SensorData(
        boolean error,
        StateClifton stateClifton,
        byte waypoint,
        short windDirection,
        float windSpeed,
        short wingSurface,
        float velocity,
        short x,
        short y,
        short heading,
        double latitude,
        double longitude,

        // debug info
        // Kartesische Koordinaten
        short targetX,
        short target_y,
        // winkel
        float targetAngle,
        float targetRelativeAngle,
        // cm
        short targetDistance,
        // relative to clifton body
        float mainSailTargetAngle

) implements Data {


    @Override
    public String toString() {
        return "SensorData{" +
                "error=" + error +
                ", stateClifton=" + stateClifton +
                ", waypoint=" + waypoint +
                ", windDirection=" + windDirection +
                ", windSpeed=" + windSpeed +
                ", wingSurface=" + wingSurface +
                ", velocity=" + velocity +
                ", x=" + x +
                ", y=" + y +
                ", heading=" + heading +
                ", latitude=" + latitude +
                ", longitude=" + longitude +
                ", targetX=" + targetX +
                ", target_y=" + target_y +
                ", targetAngle=" + targetAngle +
                ", targetRelativeAngle=" + targetRelativeAngle +
                ", targetDistance=" + targetDistance +
                ", mainSailTargetAngle=" + mainSailTargetAngle +
                '}';
    }
}
