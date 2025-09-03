import { getCliftonService } from '../api/services';
import { useErrorHandler } from './errorHandler';

// Get immediate data (fallback when API is not available)
export const getImmediateData = () => {
    return {
        windSpeed : 13,
        windDirection : 13,
        latitude : 190,
        longitude : 190,
        velocity : 190,
        heading : 190,
        state : 1,
        error : false,
        waypoint : 2,
        targetX: 1,
        targetY: 1,
        targetAngle: 1,
        targetDistance: 1,
        targetRelativeAngle: 1,
        mainSailTargetAngle: 1,
        wingSurface: 1
    };
};

// Fetch analytics data from the backend
export const fetchCliftonData = async () => {
    const errorHandler = useErrorHandler();

    try {
        const clifton = await getCliftonService();

        return {
            windSpeed: clifton.windSpeed,
            windDirection: clifton.windDirection,
            latitude: clifton.latitude,
            longitude: clifton.longitude,
            velocity: clifton.velocity,
            heading: clifton.heading,
            state: clifton.state,
            error: clifton.error,
            waypoint: clifton.waypoint,
            targetX: clifton.targetX,
            targetY: clifton.targetY,
            targetAngle: clifton.targetAngle,
            targetDistance: clifton.targetDistance,
            targetRelativeAngle: clifton.targetRelativeAngle,
            mainSailTargetAngle: clifton.mainSailTargetAngle,
            wingSurface: clifton.wingSurface,
        };
    } catch (error) {
        errorHandler.handleApiError(error, 'Error loading clifton data');
        return getImmediateData();
    }
};
