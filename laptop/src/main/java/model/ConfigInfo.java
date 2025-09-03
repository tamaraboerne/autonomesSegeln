package model;

public record ConfigInfo(
        double sailSpeedTolerance,
        double sailSpeedConstant,
        float flapStep,
        double steeringConstantP,
        byte defaultWaypointTolerance,
        byte curveWaypointTolerance
) {

    @Override
    public String toString() {
        return "ConfigInfo{" +
                "sailSpeedTolerance=" + sailSpeedTolerance +
                ", sailSpeedConstant=" + sailSpeedConstant +
                ", flapStep=" + flapStep +
                ", steeringConstantP=" + steeringConstantP +
                ", defaultWaypointTolerance=" + defaultWaypointTolerance +
                ", curveWaypointTolerance=" + curveWaypointTolerance +
                '}';
    }
}
