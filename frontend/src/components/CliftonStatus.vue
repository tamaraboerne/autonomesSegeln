<template>
  <div class="status-wrapper">
    <div class="status-box">
      <strong>Status:</strong> {{ statusLabel }}
    </div>

    <div v-if="isError" class="error-box">
      <strong>Error Occurred</strong>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { cliftonStatus, isError } from '@/utils/debugState'

/** mapping for clifton status codes from integer to string */
const statusMap = {
  0: 'Idle',
  1: 'Ready',
  2: 'Driving',
} as const

/** Computed label for the current Clifton status.
 * Falls back to "Unknown" if the status is null or unmapped */
const statusLabel = computed(() => {
  return cliftonStatus.value !== null
      ? statusMap[cliftonStatus.value as keyof typeof statusMap]
      : 'Unknown'
})
</script>

<style scoped>
.status-wrapper {
  position: fixed;
  top: 1rem;
  left: 50%;
  transform: translateX(-50%);
  display: flex;
  gap: .5rem;
  z-index: 3000;
}

.status-box,
.error-box {
  padding: 0.5rem 1rem;
  border-radius: 8px;
  font-weight: bold;
  font-size: 1rem;
  color: white;
  backdrop-filter: blur(1px);
}

.status-box {
  background-color: rgba(0, 0, 0, 0.7);
}

.error-box {
  background-color: rgba(200, 0, 0, 0.7);
}
</style>