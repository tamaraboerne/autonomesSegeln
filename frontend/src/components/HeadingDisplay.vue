<script setup lang="ts">
import { computed } from "vue";
import type { LatLngTuple } from "leaflet";
import { LPolyline } from "@vue-leaflet/vue-leaflet";

// ⚠️ UNUSED ⚠️️

const props = defineProps<{
  position: LatLngTuple;
  heading: number; // in degrees, nautical (0 = North)
}>();


/** length of heading lines (in meter)*/
const length = 3;

/**
 * Computes a destination point from a starting lat/lng, given a heading and distance.
 *
 * Uses a simple equirectangular approximation (good for small distances).
 *
 * @param lat - Latitude of the start point (in degrees).
 * @param lng - Longitude of the start point (in degrees).
 * @param angleDeg - Heading angle in degrees (0° = North).
 * @param distance - Distance to move in meters.
 * @returns The new [lat, lng] destination point.
 */
function computeTargetPoint(lat: number, lng: number, angleDeg: number, distance: number): LatLngTuple {
  const R = 6371000; // Earth-Radius
  const angleRad = angleDeg * Math.PI / 180;

  const dLat = (distance * Math.cos(angleRad)) / R;
  const dLng = (distance * Math.sin(angleRad)) / (R * Math.cos(lat * Math.PI / 180));

  const newLat = lat + dLat * (180 / Math.PI);
  const newLng = lng + dLng * (180 / Math.PI);

  return [newLat, newLng];
}

/** Main heading line (red), showing exact heading direction */
const headingLine = computed(() => [
  props.position,
  computeTargetPoint(props.position[0], props.position[1], props.heading, length)
]);

/** Secondary line (+8.25°), orange dashed */
const plusLine = computed(() => [
  props.position,
  computeTargetPoint(props.position[0], props.position[1], props.heading + 8.25, length)
]);

/** Secondary line (-8.25°), orange dashed */
const minusLine = computed(() => [
  props.position,
  computeTargetPoint(props.position[0], props.position[1], props.heading - 8.25, length)
]);
</script>

<template>
  <div>
    <l-polyline :lat-lngs="headingLine" color="red" :weight="2" />
    <l-polyline :lat-lngs="plusLine" color="orange" :weight="2" dashArray="4" />
    <l-polyline :lat-lngs="minusLine" color="orange" :weight="2" dashArray="4" />
  </div>
</template>

<style scoped>

</style>