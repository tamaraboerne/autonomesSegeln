<script setup lang="ts">
/**
 * This component displays:
 * - the real wing direction as a rotating SVG arrow
 * - the current wing rotation in degree
 *
 * The data comes from the global state via `debugState.ts`.
 */

import {mainWingSurface} from '@/utils/debugState';
</script>

<template>
  <div class="wing-display">
    <div class="label">Wing Direction</div>

    <div class="circle">
      <svg
          class="arrow"
          :style="{ transform: 'rotate(' + mainWingSurface + 'deg)' }"
          xmlns="http://www.w3.org/2000/svg"
          viewBox="0 0 24 30"
          width="60"
          height="80"
      >
        <path
            fill="white"
            d="M12 1L16 9H13V29H11V9H8L12 1Z"
        />
      </svg>
    </div>

    <div class="speed">{{ mainWingSurface.toFixed(1) }} °</div>
  </div>
</template>

<style scoped>
.wing-display {
  position: absolute;
  top: 600px;
  left: 10px;
  background-color: rgba(30, 30, 30, 0.8);
  padding: 20px;
  border-radius: 6px;
  color: #fff;
  text-align: center;
  width: 180px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.6);
  font-family: sans-serif;
  z-index: 1000;
}

.label {
  font-weight: bold;
  margin-bottom: 14px;
  font-size: 16px;
}

.circle {
  background-color: rgba(30, 30, 30, 0.70);
  border-radius: 50%;
  width: 120px;
  height: 120px;
  margin: 0 auto 14px auto;
  display: flex;
  align-items: center;
  justify-content: center;
}

.arrow {
  transition: transform 0.5s ease;
}

.speed {
  font-size: 16px;
  color: #fff;
}
</style>