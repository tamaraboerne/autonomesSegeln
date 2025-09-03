// Base URL for the backend
export const API_BASE_URL = 'http://localhost:8080/api';

// API Endpoints
export const ENDPOINTS = {
    CLIFTON: `${API_BASE_URL}/clifton`,
    ROUTE: `${API_BASE_URL}/route`,
    START: `${API_BASE_URL}/start`,
    STOP: `${API_BASE_URL}/stop`,
    HEALTH: `${API_BASE_URL}/health`,
    DEBUG_CLIFTON: `${API_BASE_URL}/debug/clifton`,
    DEBUG_BACKEND: `${API_BASE_URL}/debug/backend`,
};

// Data types
export interface Clifton {
    windSpeed: number;
    windDirection: number;
    latitude: number;
    longitude: number;
    velocity: number;
    heading: number;
    state: number;
    error: boolean;
    waypoint: number;
    targetX: number;
    targetY: number;
    targetAngle: number;
    targetDistance: number;
    targetRelativeAngle: number;
    mainSailTargetAngle: number;
    wingSurface: number;
}
