<script setup lang="ts">
import { ref } from 'vue';
const isOpen = ref(true);

// ⚠️ UNUSED ⚠️️

/** toggle for opening the legend sidebar to hide it */
function toggleSidebar() {
  isOpen.value = !isOpen.value;
}
</script>

<template>
  <div class="legend-sidebar" :class="{ open: isOpen }">
    <button class="toggle-button" @click="toggleSidebar">
      {{ isOpen ? '⟨' : '⟩' }}
    </button>
    <div v-if="isOpen" class="content">
      <h3>Legende</h3>
      <ul>
        <li>
          <span class="line route-line"></span>
          Geplante Route
        </li>
        <li>
          <span class="line heading-line"></span>
          Richtung von Clifton (Heading)
        </li>
        <li>
          <span class="line help-line"></span>
          Perfekter Vortriebswinkel (±8,25°)
        </li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.legend-sidebar {
  position: absolute;
  top: 75%;
  left: 0;
  width: 260px;
  background-color: rgba(0, 0, 0, 0.7);
  color: #fff;
  font-family: sans-serif;
  padding: 15px 20px;
  border-radius: 0 12px 12px 0;
  backdrop-filter: blur(4px);
  transition: transform 0.3s ease;
  transform: translateX(-100%);
  z-index: 1001;
  box-shadow: 4px 0 10px rgba(0, 0, 0, 0.4);
}

.legend-sidebar.open {
  transform: translateX(0);
}

.toggle-button {
  position: absolute;
  top: 10px;
  right: -35px;
  width: 35px;
  height: 35px;
  background: rgba(0, 0, 0, 0.7);
  border: none;
  border-radius: 50%;
  cursor: pointer;
  color: #fff;
  font-weight: bold;
  font-size: 1.2em;
  box-shadow: 2px 2px 5px rgba(0,0,0,0.3);
}

.content h3 {
  margin-top: 0;
  font-size: 1.2em;
  color: #ffffff;
}

ul {
  list-style: none;
  padding: 0;
  margin: 0;
}

li {
  margin-bottom: 12px;
  display: flex;
  align-items: center;
  font-size: 0.95em;
  color: #ffffff;
}

.line {
  display: inline-block;
  width: 30px;
  height: 4px;
  margin-right: 10px;
  vertical-align: middle;
  border-radius: 2px;
}
.route-line {
  background-color: var(--route-color);
}

.heading-line {
  background-color: var(--heading-color);
}

.help-line {
  background-image: var(--help-line-pattern);
}


.help-line {
  background-image: repeating-linear-gradient(
      to right,
      orange,
      orange 4px,
      transparent 4px,
      transparent 8px
  );
  background-size: 8px 100%;
  background-repeat: repeat-x;
}
</style>
