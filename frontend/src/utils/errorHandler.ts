import { useToastService } from './toastService';

/**
 * Global error handling service that uses toast notifications
 */
export function useErrorHandler() {
    const toast = useToastService();

    /**
     * Handler for API errors
     * @param error The error that occurred
     * @param defaultMessage Default error message
     */
    const handleApiError = (error: unknown, defaultMessage = 'An error has occurred') => {
        if (error instanceof Error) {
            if (error.message.includes('network') || error.message.includes('connection')) {
                toast.error('Network error: Please check your connection');
            } else if (error.message.includes('timeout')) {
                toast.error('The request took too long. Please try again later');
            } else if (error.message.includes('server')) {
                toast.error('Server error: Please make sure the server is running');
            } else {
                toast.error(`Error: ${error.message}`);
            }
        } else {
            toast.error(defaultMessage);
        }
    };

    return {
        handleApiError,
    };
}
