<template>
  <div class="tuning-panel" :style="tuningPanelStyle">
    <h3>Control parameters</h3>

    <!-- Input fields -->
    <label>Sail Speed Tolerance:
      <input type="number" step="0.01" v-model.number="sailSpeedTolerance" />
    </label>

    <label>Sail Speed Constant:
      <input type="number" step="0.01" v-model.number="sailSpeedConstant" />
    </label>

    <label>Steering Constant P:
      <input type="number" step="0.01" v-model.number="steeringConstantP" />
    </label>

    <label>Flap Step:
      <input type="range" min="0" max="0.5" step="0.01" v-model.number="flapStep" />
      <span>{{ flapStep.toFixed(2) }}</span>
    </label>

    <label>Default Waypoint Tolerance:
      <input type="number" v-model.number="defaultWaypointTolerance" />
    </label>

    <label>Curve Waypoint Tolerance:
      <input type="number" v-model.number="curveWaypointTolerance" />
    </label>

    <button @click="submitParameters">Senden</button>

    <!-- Live data -->
    <h4>Zielpunkt</h4>
    <div>X: {{ targetX }} cm</div>
    <div>Y: {{ targetY }} cm</div>
    <div>Distanz: {{ targetDistance }} cm</div>
    <div>Winkel abs: {{ targetAngle }}°</div>
    <div>Winkel rel: {{ targetRelativeAngle }}°</div>
    <div>Segel Zielwinkel: {{ sailTargetAngle }}°</div>
  </div>
</template>

<script setup lang="ts">
import {computed, ref} from 'vue'
import {
  targetX, targetY, targetDistance,
  targetAngle, targetRelativeAngle,
  mainSailTargetAngle as sailTargetAngle, isInDebugMode
} from '@/utils/debugState'
import { sendTuningParameters } from '@/api/services.ts'

// Tuning Values
const sailSpeedTolerance = ref(0.0)
const sailSpeedConstant = ref(1.0)
const steeringConstantP = ref(1.0)
const flapStep = ref(0.01)
const defaultWaypointTolerance = ref(50)
const curveWaypointTolerance = ref(75)

/** submits tuning vals separate to debug */
const submitParameters = () => {
  sendTuningParameters(
    sailSpeedTolerance.value,
    sailSpeedConstant.value,
    steeringConstantP.value,
    flapStep.value,
    defaultWaypointTolerance.value,
    curveWaypointTolerance.value
  )
}

/** moves panel a bit to the right if debug panel is open, to prevent overlapping panels */
const tuningPanelStyle = computed(() => {
  return {
    right: isInDebugMode.value ? '20rem' : '1rem'
  }
})

</script>

<style scoped>

.tuning-panel {
  position: fixed;
  bottom: 1rem;
  right: 1rem;
  width: 300px;
  padding: 1rem;
  background-color: rgba(20, 20, 20, 0.85);
  background-color: rgba(20, 20, 20, 0.85);
  color: white;
  border-radius: 12px;
  box-shadow: 0 0 10px rgba(0,0,0,0.5);
  z-index: 1500;
  font-size: 0.85rem;
}
label {
  display: flex;
  flex-direction: column;
  margin-bottom: 0.6rem;
}
button {
  margin-top: 0.5rem;
  background: #444;
  color: white;
  padding: 0.4rem 0.8rem;
  border: none;
  border-radius: 6px;
  cursor: pointer;
}
button:hover {
  background: #666;
}
</style>
