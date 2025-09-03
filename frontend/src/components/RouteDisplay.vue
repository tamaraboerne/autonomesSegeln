<script setup lang="ts">
import {computed, defineProps, ref} from 'vue';
import type { LatLngTuple } from 'leaflet';
import {LCircleMarker, LPolyline} from "@vue-leaflet/vue-leaflet";
import { liveWaypoint } from "@/utils/debugState.ts";

/**
 * Reads a CSS variable value from the root document.
 * Used to have on file for the changeable colors.
 *
 * @param variable - The CSS variable name, e.g., '--route-color'.
 * @returns The computed CSS value as a trimmed string.
 */
function getCssVar(variable: string): string {
  return getComputedStyle(document.documentElement).getPropertyValue(variable).trim();
}

/** reactive colors based on CSS variables */
const routeColor = ref(getCssVar('--route-color'));
const routeColorDone = ref(getCssVar('--route-color-done'));

/**
 * Component props:
 * - `route`: An array of LatLng tuples defining the entire route.
 */
const props = defineProps<{
  route: LatLngTuple[];
}>();

/**
 * Filters out invalid coordinates.
 * Only includes tuples of exactly two numeric values.
 */
const validPoints = computed(() =>
    props.route?.filter(point =>
        point &&
        point.length === 2 &&
        typeof point[0] === 'number' &&
        typeof point[1] === 'number'
    ) || []
);

/** Subset of `validPoints` that have already been passed (up to `liveWaypoint` index) */
const donePoints = computed(() => validPoints.value.slice(0, liveWaypoint.value));

/** Remaining points on the route after the current `liveWaypoint` index */
const upcomingPoints = computed(() => validPoints.value.slice(liveWaypoint.value));
</script>

<template>
  <div>
    <l-polyline
        v-if="validPoints.length >= 2"
        :lat-lngs="validPoints"
        :color="routeColor"
        :weight="3"
        :smoothFactor="1"
    />

    <!-- already done points - orange points -->
    <l-circle-marker
        v-for="(point, index) in donePoints"
        :key="'done-' + index"
        :lat-lng="point"
        :radius="4"
        :color="routeColorDone"
        :fillColor="routeColorDone"
        :fillOpacity="1"
    />

    <!-- upcomingPoints - purple points -->
    <l-circle-marker
        v-for="(point, index) in upcomingPoints"
        :key="'upcoming-' + index"
        :lat-lng="point"
        :radius="4"
        :color="routeColor"
        :fillColor="routeColor"
        :fillOpacity="1"
    />
  </div>
</template>

<style scoped>

</style>