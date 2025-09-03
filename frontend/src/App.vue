<script setup lang="ts">
import {LCircleMarker, LMap, LMarker, LTileLayer} from "@vue-leaflet/vue-leaflet";
import {DivIcon, LatLngTuple, Polygon, LatLngExpression, Map} from 'leaflet';
import L, {Icon} from 'leaflet';
import "leaflet/dist/leaflet.css";
import {computed, onMounted, onUnmounted, ref, watch} from "vue";
import {fetchCliftonData} from "@/utils/data.ts";
import Button from "./components/Button.vue";
import WindDisplay from "@/components/WindDisplay.vue";
import RouteDisplay from "@/components/RouteDisplay.vue";
import {sendRouteCommand, sendStartCommand, sendStopCommand} from "@/api/services.ts";
import Debug from "@/components/Debug.vue";
import {
  liveCliftonLat,
  liveCliftonLng,
  liveHeading,
  liveWindSpeed,
  liveWindDirection,
  targetLat,
  targetLng,
  cliftonStatus,
  isError,
  liveWaypoint,
  targetX,
  targetRelativeAngle,
  mainSailTargetAngle,
  targetY,
  targetAngle, targetDistance, mainWingSurface
} from '@/utils/debugState';
import {useToastService} from "@/utils/toastService.ts";
import CliftonStatus from "@/components/CliftonStatus.vue";
import ControlTuningANDViewPanel from "@/components/ControlTuningANDViewPanel.vue";
import WingDisplay from "@/components/WingDisplay.vue";

/** map init state */
const zoom = ref<number>(18);
const audimaxLat = ref<number>(53.834669);
const audimaxLng = ref<number>(10.699215);
const mapRef = ref()

/** init toast msgs */
const toast = useToastService();

/** v-if helper func for when to show the target pos marker */
const clickedMarkerLatLng = computed<LatLngTuple | null>(() => {
  if (targetLat.value !== null && targetLng.value !== null) {
    return [targetLat.value, targetLng.value];
  }
  return null;
});

/** Clifton icon with live heading rotation */
const cliftonIcon = computed<DivIcon>(() => {
  return L.divIcon({
    className: 'clifton-icon',
    html: `
      <div style="transform: rotate(${liveHeading.value}deg);
                 transform-origin: center;
                 transition: transform 0.2s ease;
                 width: 48px;
                 height: 48px;">
        <img src="clifton.svg" style="width: 100%; height: 100%;"  alt="clifton"/>
      </div>
    `,
    iconSize: [48, 48],
    iconAnchor: [24, 24]
  });
});

const cliftonMarker = ref<any>(null);

/** Target icon, if clicked on map */
const targetIcon = computed<Icon>(() => new L.Icon({
  iconUrl: '/public/target.svg',
  iconSize: [48, 48],
  iconAnchor: [26, 48],
  popupAnchor: [0, -48],
  className: 'target-icon'
}));

/** handles map click: store clicked coords as target */
function onMapClick(e: any) {
  const { lat, lng } = e.latlng;
  targetLat.value = lat;
  targetLng.value = lng;
  console.log("Clicked coordinates:", lat, lng);
}

/** resets target values to null ==> targetmarker disappears on map */
function removeClickedMarker() {
  targetLat.value = null;
  targetLng.value = null;
}

/** live clifton marker */
let markerLatLng = computed<LatLngTuple>(() => [liveCliftonLat.value, liveCliftonLng.value]);

/** defines where map is focused on begin */
const center = ref<[number, number]>([audimaxLat.value, audimaxLng.value]);

/** Validate that Clifton location is not zero,prevent clifton to appear in the ocean on coord [0, 0] */
let isMarkerValid = computed(() => {return markerLatLng.value[0] !== 0 && markerLatLng.value[1] !== 0});

/** Fetch and update the current data for the Clifton */
const updateCliftonData = async () => {
  try {
    const clifton = await fetchCliftonData();
    liveCliftonLat.value = clifton.latitude;
    liveCliftonLng.value = clifton.longitude;
    liveHeading.value = clifton.heading;
    liveWindSpeed.value = clifton.windSpeed;
    liveWindDirection.value = clifton.windDirection;
    cliftonStatus.value = clifton.state;
    isError.value = clifton.error;
    liveWaypoint.value = clifton.waypoint;
    targetX.value = clifton.targetX;
    targetY.value = clifton.targetY;
    targetAngle.value = clifton.targetAngle;
    targetDistance.value = clifton.targetDistance;
    targetRelativeAngle.value = clifton.targetRelativeAngle;
    mainSailTargetAngle.value = clifton.mainSailTargetAngle;
    mainWingSurface.value = clifton.wingSurface;
    mainWingSurface.value = clifton.wingSurface;
  } catch (error) {
    console.error("Error updating Clifton data:", error);
  }
};

/** Automatically start data updates when component is mounted */
onMounted(async () => {
  // Initial data fetch
  await updateCliftonData();

  // Set up interval to update data every second
  const intervalId = setInterval(updateCliftonData, 1000); // TODO: refresh data > ACTIVATE IN PROD

  // Clear interval on component unmount
  return () => clearInterval(intervalId); // TODO: refresh data > ACTIVATE IN PROD
});

/** sends Stop command if STOP button is pressed */
function sendStop(_code: string) {
  sendStopCommand();
}

/** sends Start command if START button is pressed */
function sendStart(_code: string) {
  sendStartCommand();
}

/** Toggle between route planning and route clearing */
function toggleRoute(_code: string) {
  if (route.value.length > 0) {
    // reset route
    route.value = [];
    targetLat.value = null;
    targetLng.value = null;
  } else if (targetLat.value !== null && targetLng.value !== null) {
    // plan route
    fetchRoute();
  } else {
    // console.warn("No target-pos selected!");
    toast.warning("No Target Selected!");
  }
}

/** Fetch a planned route from current location to the selected target-pos */
async function fetchRoute() {
  try {
    const response = await sendRouteCommand(targetLat.value!, targetLng.value!);

    route.value = response;

    console.log("route:", route.value);
  } catch (error) {
    console.error("Failed to fetch route:", error);
  }
}

/** route field, stores route points from route team */
let route = ref<LatLngTuple[]>([]);

/** Toggle field for displaying the polygon showing the defined sailing area */
const showSailingArea = ref(false)

/** area coords from sailing-area-team*/
const sailingAreaCoords: LatLngExpression[] = [
  [53.83447994, 10.69952499],
  [53.83479481, 10.70054092],
  [53.83472304, 10.70060538],
  [53.83432977, 10.69942065],
  [53.83509392, 10.69868656],
  [53.83539367, 10.69954897],
  [53.83521470, 10.69969939],
  [53.83498565, 10.69901467]
]

/** Polygon field for storing the sailing area */
let sailingPolygon: Polygon | null = null

/** Watcher to draw/remove sailing area polygon*/
watch(showSailingArea, (visible) => {
  const leafletMap: Map | undefined = mapRef.value?.leafletObject
  if (!leafletMap) return

  if (visible) {
    sailingPolygon = new Polygon(sailingAreaCoords, {
      color: 'lime',
      fillColor: 'lime',
      fillOpacity: 0.2
    }).addTo(leafletMap)
  } else {
    sailingPolygon?.remove()
    sailingPolygon = null
  }
})

// vvv Clifton position history logging vvv
const showCliftonHistory = ref(false);
const cliftonHistory = ref<LatLngTuple[]>([]);
let cliftonHistoryInterval: ReturnType<typeof setInterval> | null = null;

/** save current position to history every 2 seconds, ignoring invalid (and exactly same) points */
onMounted(() => {
  cliftonHistoryInterval = setInterval(() => {
    const lat = liveCliftonLat.value;
    const lng = liveCliftonLng.value;

    // filter: no Default-values 190 or init-values 0,0
    if (lat > 60 || lng > 60 || lat === 0 || lng === 0) return;

    // save only new values + no duplicates
    const last = cliftonHistory.value.at(-1);
    if (!last || last[0] !== lat || last[1] !== lng) {
      cliftonHistory.value.push([lat, lng]);
      console.log(cliftonHistory.value);
    }
  }, 2000);
});

/** clear history interval on unmount */
onUnmounted(() => {
  if (cliftonHistoryInterval) clearInterval(cliftonHistoryInterval);
});

/** reset history points manually */
function resetCliftonHistory() {
  cliftonHistory.value = [];
}
</script>

<template>
<!--  <LegendSidebar />-->
  <ControlTuningANDViewPanel />
  <CliftonStatus />
  <Debug />

<!-- map / tiles -->
  <div class="map-container">
    <l-map ref="mapRef" v-model:zoom="zoom" v-model:center="center" :useGlobalLeaflet="false" @click="onMapClick">
      <l-tile-layer
          url="/tiles/cycle/{z}/{x}/{y}.png"
          attribution="&copy; Thunderforest, OpenStreetMap contributors"
          :minZoom="17"
          :maxZoom="21"
      />
      <RouteDisplay :route="route" />
      <l-marker
          v-if="isMarkerValid"
          ref="cliftonMarker"
          :lat-lng="markerLatLng"
          :icon="cliftonIcon as any"
      ></l-marker>
      <l-marker v-if="clickedMarkerLatLng" :lat-lng="clickedMarkerLatLng" @contextmenu="removeClickedMarker" :icon="targetIcon"></l-marker>

      <!-- Clifton history -->
      <l-circle-marker
          v-if="showCliftonHistory"
          v-for="(point, index) in cliftonHistory"
          :key="index"
          :lat-lng="point"
          :radius="3"
          color="orange"
          fill-color="orange"
          :fill-opacity="0.7"
      />

    </l-map>
  </div>

  <div class="settings-panel">
    <h2 class="title">UI Utils</h2>

    <!-- sailing area toggle -->
    <div class="option">
      <span>Show Sailing Area</span>
      <label class="switch">
        <input type="checkbox" v-model="showSailingArea" />
        <span class="slider"></span>
      </label>
    </div>


    <div class="option">
      <span>Clifton History</span>
      <label class="switch">
        <input type="checkbox" v-model="showCliftonHistory" />
        <span class="slider"></span>
      </label>
      <button @click="resetCliftonHistory" class="reset-button">⟲</button>
    </div>
  </div>

  <WindDisplay />
  <WingDisplay />

  <div class="button-panel">
    <Button
        :text="route.length > 0 ? 'RESET ROUTE' : 'PLAN ROUTE'"
        code="TOGGLE_ROUTE"
        @trigger="toggleRoute"
    />
    <Button
        text="START"
        code="START"
        bgColor="#27ae60"
        textColor="#ffffff"
        @trigger="sendStart"
    />
    <Button
        text="STOP"
        code="STOP"
        bgColor="#c0392b"
        textColor="#ffffff"
        @trigger="sendStop"
    />
  </div>
</template>

<style>
.map-container {
  height: 100vh;
  width: 100vw;
}
.button-panel {
  position: absolute;
  bottom: 10%;
  left: 50%;
  transform: translateX(-50%);
  display: flex;
  flex-direction: row;
  gap: 20px;
  z-index: 1000;
}
.clifton-icon img {
  background: none !important;
  border: none !important;
}

.settings-panel {
  position: fixed;
  top: 50%;
  left: 0;
  width: 260px;
  background-color: rgba(30, 30, 30, 0.8);
  color: white;
  padding: 1rem;
  z-index: 1500;
  border-right: 1px solid #444;
  border-radius: 10px;
  box-shadow: 2px 0 10px rgba(0, 0, 0, 0.3);
  transform: translateY(-50%);
}

.title {
  font-size: 1.25rem;
  font-weight: bold;
  margin-bottom: 1.5rem;
}

.option {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
}

.switch {
  position: relative;
  display: inline-block;
  width: 42px;
  height: 24px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #888;
  border-radius: 24px;
  cursor: pointer;
  transition: background-color 0.3s;
}

.slider::before {
  content: "";
  position: absolute;
  height: 18px;
  width: 18px;
  left: 3px;
  bottom: 3px;
  background-color: white;
  border-radius: 50%;
  transition: transform 0.3s;
}

input:checked + .slider {
  background-color: #00c17c;
}

input:checked + .slider::before {
  transform: translateX(18px);
}

.reset-button {
  background: none;
  border: 1px solid #ccc;
  border-radius: 5px;
  padding: 2px 6px;
  color: white;
  margin-left: 8px;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background-color 0.3s;
}

.reset-button:hover {
  background-color: rgba(255, 255, 255, 0.1);
}
</style>
