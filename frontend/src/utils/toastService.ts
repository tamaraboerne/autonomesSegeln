// Toast types
export enum ToastType {
    SUCCESS = 'success',
    ERROR = 'error',
    WARNING = 'warning',
    INFO = 'info'
}

// For TypeScript to recognize WebKit prefixes
declare global {
    interface CSSStyleDeclaration {
        webkitBackdropFilter: string;
    }
}

// Default toast options
const defaultOptions = {
    position: 'top-right',
    timeout: 3000,
    closeOnClick: false // Changed to false since we'll have a separate close button
};

// Toast class for DOM manipulation
class ToastManager {
    private container: HTMLDivElement | null = null;
    private toasts: Map<string, { element: HTMLDivElement, timeoutId?: ReturnType<typeof setTimeout> }> = new Map();
    private counter: number = 0;

    constructor() {
        if (typeof document !== 'undefined') {
            this.createContainer();
        }
    }

    private createContainer() {
        // If container already exists
        if (this.container) return;

        // Create container
        this.container = document.createElement('div');
        this.container.className = 'toast-container';
        this.container.style.position = 'fixed';
        this.container.style.top = '1rem';
        this.container.style.right = '1rem';
        this.container.style.zIndex = '9999';
        this.container.style.display = 'flex';
        this.container.style.flexDirection = 'column';
        this.container.style.gap = '0.5rem';
        document.body.appendChild(this.container);
    }

    // Show toast
    show(message: string, type: ToastType = ToastType.INFO, options = {}) {
        if (typeof document === 'undefined' || !this.container) return;

        const opts = { ...defaultOptions, ...options };
        const id = `toast-${this.counter++}`;

        // Toast container
        const toast = document.createElement('div');
        toast.id = id;
        toast.className = `toast toast-${type}`;

        // Toast content wrapper with message
        const contentWrapper = document.createElement('div');
        contentWrapper.className = 'toast-content';
        contentWrapper.textContent = message;

        // Close button
        const closeButton = document.createElement('button');
        closeButton.className = 'toast-close';
        closeButton.innerHTML = '&times;';
        closeButton.style.position = 'absolute';
        closeButton.style.top = '0.5rem';
        closeButton.style.right = '0.5rem';
        closeButton.style.background = 'transparent';
        closeButton.style.border = 'none';
        closeButton.style.color = 'rgba(255, 255, 255, 0.8)';
        closeButton.style.fontSize = '1rem';
        closeButton.style.fontWeight = 'bold';
        closeButton.style.cursor = 'pointer';
        closeButton.style.padding = '0';
        closeButton.style.width = '20px';
        closeButton.style.height = '20px';
        closeButton.style.display = 'flex';
        closeButton.style.alignItems = 'center';
        closeButton.style.justifyContent = 'center';
        closeButton.style.borderRadius = '50%';
        closeButton.style.transition = 'background-color 0.2s ease';
        closeButton.addEventListener('mouseenter', () => {
            closeButton.style.backgroundColor = 'rgba(255, 255, 255, 0.1)';
        });
        closeButton.addEventListener('mouseleave', () => {
            closeButton.style.backgroundColor = 'transparent';
        });
        closeButton.addEventListener('click', (e) => {
            e.stopPropagation();
            this.dismiss(id);
        });

        // Progress bar
        const progressBar = document.createElement('div');
        progressBar.className = 'toast-progress';
        progressBar.style.position = 'absolute';
        progressBar.style.bottom = '0';
        progressBar.style.left = '0';
        progressBar.style.height = '3px';
        progressBar.style.width = '100%';
        progressBar.style.transformOrigin = 'left';
        progressBar.style.transition = `width ${opts.timeout}ms linear`;

        // Set progress bar color based on type
        switch (type) {
            case ToastType.SUCCESS:
                progressBar.style.background = 'rgba(255, 255, 255, 0.7)';
                break;
            case ToastType.ERROR:
                progressBar.style.background = 'rgba(255, 255, 255, 0.7)';
                break;
            case ToastType.WARNING:
                progressBar.style.background = 'rgba(255, 255, 255, 0.7)';
                break;
            case ToastType.INFO:
            default:
                progressBar.style.background = 'rgba(255, 255, 255, 0.7)';
                break;
        }

        // Styling for toast container
        toast.style.padding = '0.75rem 1rem';
        toast.style.paddingRight = '2rem'; // Extra space for close button
        toast.style.borderRadius = '0.375rem';
        toast.style.boxShadow = '0 4px 6px -1px rgba(0, 0, 0, 0.2), 0 2px 4px -1px rgba(0, 0, 0, 0.1)';
        toast.style.marginBottom = '0.5rem';
        toast.style.position = 'relative';
        toast.style.overflow = 'hidden'; // For progress bar
        toast.style.opacity = '0';
        toast.style.transform = 'translateX(100%)'; // Start from right (horizontal animation)
        toast.style.transition = 'opacity 0.3s, transform 0.3s';

        // Colors based on type
        switch (type) {
            case ToastType.SUCCESS:
                toast.style.backgroundColor = '#059669';
                toast.style.color = '#ffffff';
                break;
            case ToastType.ERROR:
                toast.style.backgroundColor = '#DC2626';
                toast.style.color = '#ffffff';
                break;
            case ToastType.WARNING:
                toast.style.backgroundColor = '#F59E0B';
                toast.style.color = '#ffffff';
                break;
            case ToastType.INFO:
            default:
                toast.style.backgroundColor = '#3B82F6';
                toast.style.color = '#ffffff';
                break;
        }

        // Add backdrop blur for modern look
        toast.style.backdropFilter = 'blur(4px)';
        toast.style.webkitBackdropFilter = 'blur(4px)';
        toast.style.backgroundColor = type === ToastType.SUCCESS ? 'rgba(5, 150, 105, 0.9)' :
            type === ToastType.ERROR ? 'rgba(220, 38, 38, 0.9)' :
                type === ToastType.WARNING ? 'rgba(245, 158, 11, 0.9)' :
                    'rgba(59, 130, 246, 0.9)';

        // Add border to match site style
        toast.style.border = '1px solid';
        toast.style.borderColor = type === ToastType.SUCCESS ? 'rgba(5, 150, 105, 0.7)' :
            type === ToastType.ERROR ? 'rgba(220, 38, 38, 0.7)' :
                type === ToastType.WARNING ? 'rgba(245, 158, 11, 0.7)' :
                    'rgba(59, 130, 246, 0.7)';

        // Click event for closing (if option is enabled)
        if (opts.closeOnClick) {
            toast.addEventListener('click', () => this.dismiss(id));
        }

        // Append elements
        toast.appendChild(contentWrapper);
        toast.appendChild(closeButton);
        toast.appendChild(progressBar);

        // Insert into DOM
        this.container.appendChild(toast);

        // Start animation
        setTimeout(() => {
            toast.style.opacity = '1';
            toast.style.transform = 'translateX(0)';

            // Start progress bar animation
            setTimeout(() => {
                progressBar.style.width = '0';
            }, 10);
        }, 10);

        // Store toast reference
        const timeoutId = opts.timeout > 0 ?
            setTimeout(() => this.dismiss(id), opts.timeout) :
            undefined;

        this.toasts.set(id, { element: toast, timeoutId });

        return id;
    }

    // Remove toast
    dismiss(id: string) {
        const toastData = this.toasts.get(id);
        if (!toastData) return;

        const { element: toast, timeoutId } = toastData;

        // Clear timeout if exists
        if (timeoutId) {
            clearTimeout(timeoutId);
        }

        // Hide
        toast.style.opacity = '0';
        toast.style.transform = 'translateX(100%)'; // Exit to right (horizontal animation)

        // Remove from DOM after animation
        setTimeout(() => {
            if (toast.parentNode) {
                toast.parentNode.removeChild(toast);
            }
            this.toasts.delete(id);
        }, 300);
    }

    // Remove all toasts
    clear() {
        this.toasts.forEach(({ element: _, timeoutId }, id) => {
            if (timeoutId) clearTimeout(timeoutId);
            this.dismiss(id);
        });
    }
}

// Singleton instance
let toastManagerInstance: ToastManager | null = null;

/**
 * Toast service for displaying notifications
 */
export const useToastService = () => {
    if (!toastManagerInstance && typeof window !== 'undefined') {
        toastManagerInstance = new ToastManager();
    }

    /**
     * Displays a toast message
     * @param message The message to display
     * @param type The type of toast message
     * @param options Additional options
     */
    const showToast = (message: string, type: ToastType = ToastType.INFO, options = {}) => {
        return toastManagerInstance?.show(message, type, options);
    };

    // Helper functions for specific toast types
    const success = (message: string, options = {}) => showToast(message, ToastType.SUCCESS, options);
    const error = (message: string, options = {}) => showToast(message, ToastType.ERROR, options);
    const warning = (message: string, options = {}) => showToast(message, ToastType.WARNING, options);
    const info = (message: string, options = {}) => showToast(message, ToastType.INFO, options);
    const clear = () => toastManagerInstance?.clear();

    return {
        showToast,
        success,
        error,
        warning,
        info,
        clear
    };
};
