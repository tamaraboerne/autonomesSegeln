import {computed, ref} from 'vue';

/** Debug toggle */
export const isInDebugMode = ref(false);

/** Live values from api */
export const liveCliftonLat = ref(0);
export const liveCliftonLng = ref(0);
export const liveWindDirection = ref(0);
export const liveWindSpeed = ref(0);
export const liveHeading = ref(0);
export const liveWaypoint = ref(0);


/** Debug values */
export const debugCliftonLat = ref(53.834669);
export const debugCliftonLng = ref(10.699215);
export const debugHeading = ref(0);
export const debugWindDirection = ref(0);
export const debugWindSpeed = ref(0);

/** Status fields*/
export const cliftonStatus = ref<number | null>(null); // 0 = Idle, 1 = Ready, 2 = Driving
export const isError = ref<boolean>(Boolean(0));
// export const errorCode = ref(0); // for later maybe would be good to know WHAT error occurred

/** controller target coordinates */
export const targetX = ref(0); // in cm
export const targetY = ref(0); // in cm
export const targetDistance = ref(0); // in cm
export const targetAngle = ref(0); // absolut in Degree
export const targetRelativeAngle = ref(0); // relative to Clifton in Degree
export const mainSailTargetAngle = ref(0); // in Degree
export const mainWingSurface = ref(0);

/**
 * Computed values, that automatically select between Debug and Live
 */
export const activeCliftonLat = computed(() =>
    isInDebugMode.value ? debugCliftonLat.value : liveCliftonLat.value
);

export const activeCliftonLng = computed(() =>
    isInDebugMode.value ? debugCliftonLng.value : liveCliftonLng.value
);

export const activeWindDirection = computed(() =>
    isInDebugMode.value ? debugWindDirection.value : liveWindDirection.value
);

export const activeWindSpeed = computed(() =>
    isInDebugMode.value ? debugWindSpeed.value : liveWindSpeed.value
);

export const activeHeading = computed(() =>
    isInDebugMode.value ? debugHeading.value : liveHeading.value
);

/** Destination coordinates (GPS) */
export const targetLat = ref<number | null>(null);
export const targetLng = ref<number | null>(null);



