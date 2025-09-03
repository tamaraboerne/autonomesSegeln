import {ENDPOINTS} from './endpoints';
import type { Clifton } from './endpoints';
import { useToastService } from '../utils/toastService';
import {LatLngTuple} from "leaflet";

// Check backend
const checkBackendStatus = async (): Promise<boolean> => {
    try {
        const response = await fetch(ENDPOINTS.HEALTH, {
            method: 'GET',
            cache: 'no-cache',
            headers: { 'Content-Type': 'application/json' }
        });

        if (response.ok) {
            const data = await response.json();
            return data.status === 'ok';
        }

        return false;
    } catch (error) {
        console.error('Health check failed:', error);
        return false;
    }
};

// Get clifton data
export async function getCliftonService(): Promise<Clifton> {
    const toast = useToastService();

    try {
        // Check server health
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const response = await fetch(ENDPOINTS.CLIFTON, {
            method: 'GET',
        });

        if (!response.ok) {
            if (response.status >= 500) {
                throw new Error('Server error');
            }
            throw new Error('Error retrieving statistics');
        }

        return await response.json() as Clifton;
    } catch (error) {
        if (error instanceof Error) {
            if (error.message === 'Server not available') {
                toast.error('The server is not reachable. Please try again later.');
            } else if (error.message === 'Server error') {
                toast.error('A server error occurred. Please try again later.');
            } else {
                toast.error(`Clifton service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}

// Send start command
export async function sendStartCommand(): Promise<boolean> {
    const toast = useToastService();

    try {
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const response = await fetch(ENDPOINTS.START, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
        });

        if (!response.ok) {
            if (response.status >= 500) {
                throw new Error('Server error');
            }
            throw new Error('Failed to start');
        }

        toast.success('Clifton started successfully');
        return true;
    } catch (error) {
        if (error instanceof Error) {
            switch (error.message) {
                case 'Server not available':
                    toast.error('The server is not reachable. Please try again later.');
                    break;
                case 'Server error':
                    toast.error('A server error occurred. Please try again later.');
                    break;
                default:
                    toast.error(`Start service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}

// Send stop command
export async function sendStopCommand(): Promise<boolean> {
    const toast = useToastService();

    try {
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const response = await fetch(ENDPOINTS.STOP, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
        });

        if (!response.ok) {
            if (response.status >= 500) {
                throw new Error('Server error');
            }
            throw new Error('Failed to stop');
        }

        toast.success('Clifton stopped successfully');
        return true;
    } catch (error) {
        if (error instanceof Error) {
            switch (error.message) {
                case 'Server not available':
                    toast.error('The server is not reachable. Please try again later.');
                    break;
                case 'Server error':
                    toast.error('A server error occurred. Please try again later.');
                    break;
                default:
                    toast.error(`Stop service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}

// Send route command
export async function sendRouteCommand(lat: number, lon: number): Promise<LatLngTuple[]> {
    const toast = useToastService();

    try {
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const coords = { lat, lon };

        const response = await fetch(ENDPOINTS.ROUTE, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(coords),
        });

        const responseData = await response.json();

        if (!response.ok) {
            if (response.status >= 500) toast.error(responseData.error);
            return [];
        }

        const routeData: LatLngTuple[] = responseData.route;

        toast.success('Route successfully retrieved');
        return routeData;
    } catch (error) {
        if (error instanceof Error) {
            switch (error.message) {
                case 'Server not available':
                    toast.error('The server is not reachable. Please try again later.');
                    break;
                default:
                    toast.error(`Start service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}

// Send debug command
export async function sendDebugCommand(
    status: boolean = true,
    windSpeed: number = 100,
    windDirection: number = 2,
    heading: number = 2,
    latitude: number = 53.83448865896108,
    longitude: number = 10.699536949396133,
): Promise<boolean> {

    const toast = useToastService();

    try {
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const data = { status, windSpeed, windDirection, heading, latitude, longitude };

        const response = await fetch(ENDPOINTS.DEBUG_BACKEND, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data),
        });

        if (!response.ok) {
            if (response.status >= 500) {
                throw new Error('Server error');
            }
            throw new Error('Failed to stop');
        }

        toast.success('Debug data updated successfully');
        return true;
    } catch (error) {
        if (error instanceof Error) {
            switch (error.message) {
                case 'Server not available':
                    toast.error('The server is not reachable. Please try again later.');
                    break;
                case 'Server error':
                    toast.error('A server error occurred. Please try again later.');
                    break;
                default:
                    toast.error(`Debug service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}

export async function sendTuningParameters(
    sailSpeedTolerance: number = 1,
    sailSpeedConstant: number = 1,
    steeringConstantP: number = 1,
    flapStep: number = 1,
    defaultWaypointTolerance: number = 1,
    curveWaypointTolerance: number = 1,
): Promise<boolean> {
    const toast = useToastService();

    try {
        const isBackendAvailable = await checkBackendStatus();
        if (!isBackendAvailable) {
            throw new Error('Server not available');
        }

        const data = { sailSpeedTolerance, sailSpeedConstant, steeringConstantP, flapStep, defaultWaypointTolerance, curveWaypointTolerance };

        const response = await fetch(ENDPOINTS.DEBUG_CLIFTON, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data),
        });

        if (!response.ok) {
            if (response.status >= 500) {
                throw new Error('Server error');
            }
            throw new Error('Failed to stop');
        }

        toast.success('Debug data updated successfully');
        return true;
    } catch (error) {
        if (error instanceof Error) {
            switch (error.message) {
                case 'Server not available':
                    toast.error('The server is not reachable. Please try again later.');
                    break;
                case 'Server error':
                    toast.error('A server error occurred. Please try again later.');
                    break;
                default:
                    toast.error(`Debug service error: ${error.message}`);
            }
        } else {
            toast.error('An unknown error has occurred');
        }
        throw error;
    }
}