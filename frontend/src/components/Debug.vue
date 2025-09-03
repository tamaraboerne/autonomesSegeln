<template>
  <div class="debug-container">
    <img
        src="@/assets/debug-icon.svg"
        alt="Debug Icon"
        class="debug-icon"
        @click="togglePanel"
    />

    <div v-if="showPanel" class="debug-panel">
      <label>
        <input type="checkbox" v-model="isInDebugMode" />
        Enable Debug Mode
      </label>

      <div class="debug-fields" v-if="isInDebugMode">
        <h4>Clifton Position</h4>
        <label>Lat: <input v-model.number="debugCliftonLat" type="number" step="0.000001" /></label>
        <label>Lng: <input v-model.number="debugCliftonLng" type="number" step="0.000001" /></label>

        <h4>Ziel Position</h4>
        <label>
          Lat:
          <input :value="targetLat" readonly />
          <button @click="copyToClipboard(targetLat)">📋</button>
        </label>
        <label>
          Lng:
          <input :value="targetLng" readonly />
          <button @click="copyToClipboard(targetLng)">📋</button>
        </label>

        <h4>Wind</h4>
        <label>Direction: <input v-model.number="debugWindDirection" type="number" step="5" /></label>
        <label>Speed: <input v-model.number="debugWindSpeed" type="number" step="0.1" /></label>

        <h4>Heading</h4>
        <label>Angle: <input v-model.number="debugHeading" type="number" /></label>

      </div>

      <button @click="sendDebugEnabled">Submit Values</button>
    </div>
  </div>
</template>

<script setup lang="ts">
import {ref, watch} from 'vue';
import {
  isInDebugMode,
  debugCliftonLat, debugCliftonLng,
  targetLat, targetLng,
  debugWindSpeed, debugWindDirection,
  debugHeading, activeWindSpeed, activeCliftonLat, activeCliftonLng, activeWindDirection, activeHeading,
} from '@/utils/debugState';
import { sendDebugCommand } from "@/api/services.ts";

/** Controls visibility of debug panel in UI. */
const showPanel = ref(false);

/** toggles visibility of debug panel */
const togglePanel = () => {
  showPanel.value = !showPanel.value;
};

/** Sends the currently active debug values to the backend
 * with debug mode on Enabled. */
const sendDebugEnabled = () => {
  sendDebugCommand(true,
      activeWindSpeed.value,
      activeWindDirection.value,
      activeHeading.value,
      activeCliftonLat.value,
      activeCliftonLng.value,
  );
};

/** sends debug disabled with variable field (used for testing) */
const sendDebugDisabled = (status: boolean) => {
  sendDebugCommand(status);
}


/** Watcher for changes to the global `isInDebugMode` ref.
 * Triggers a debug state update on change */
watch(isInDebugMode, (newVal) => {
  sendDebugDisabled(newVal);
})

/**
* Copies a value (number, string, or null) to the clipboard.
* Falls back to the string "null" if value is null or undefined.
* @param value - The value to copy to clipboard
*/
function copyToClipboard(value: string | number | null) {
  if (value === null || value === undefined) {
    navigator.clipboard.writeText("null")
        .then(() => console.log('Copied to clipboard:', value))
        .catch(err => console.error('Copy failed:', err));
  } else {
    navigator.clipboard.writeText(value.toString())
        .then(() => console.log('Copied to clipboard:', value))
        .catch(err => console.error('Copy failed:', err));
  }
}
</script>

<style scoped>
.debug-container {
  position: fixed;
  top: 1rem;
  right: 1rem;
  z-index: 2000;
}
.debug-icon {
  width: 32px;
  height: 32px;
  cursor: pointer;
}
.debug-panel {
  margin-top: 10px;
  padding: 1rem;
  background-color: rgba(0, 0, 0, 0.85);
  color: white;
  border-radius: 10px;
  width: 250px;
}
.debug-panel input[type="number"] {
  width: 100%;
  margin-bottom: 0.5rem;
}
label {
  display: flex;
  align-items: center;
  gap: 0.25rem;
  margin-bottom: 0.5rem;
}

input[readonly] {
  flex: 1;
  background-color: #333;
  color: white;
  border: 1px solid #888;
  padding: 0.25rem;
  border-radius: 4px;
}

button {
  background: #555;
  border: none;
  color: white;
  padding: 0.2rem 0.5rem;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background: #777;
}
</style>
