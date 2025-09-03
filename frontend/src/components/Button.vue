<script setup lang="ts">
import { computed} from 'vue';

/** Component properties:
* - text: Button label text (required)
* - code: Code/identifier emitted on click (required)
* - bgColor: Background color of the button (optional, defaults to dark transparent)
* - textColor: Text color of the button (optional, defaults to white)
*/
const props = defineProps({
  text: { type: String, required: true },
  code: { type: String, required: true },
  bgColor: { type: String, default: 'rgba(30, 30, 30, 0.7)' },
  textColor: { type: String, default: '#ffffff' },
})

/**
 * Emits a "trigger" event when the button is clicked,
 * passing along the provided code prop.
 */
const emit = defineEmits(['trigger']);

/** Handles button click by emitting the trigger-event */
function handleClick() {
  emit('trigger', props.code);
}

/** computes the style for the button based on props */
const buttonStyle = computed(() => ({
  backgroundColor: props.bgColor,
  color: props.textColor,
  border: 'none',
  borderRadius: '6px',
  padding: '14px 25px',
  fontSize: '18px',
  cursor: 'pointer',
  transition: 'opacity .3s',
  boxShadow: '0 4px 10px rgba(0, 0, 0, 0.3)'

}));
</script>

<template>
  <button
    @click="handleClick"
    :style="buttonStyle"
    class="custom-button"
    >
      {{ text }}
  </button>
</template>

<style scoped>
.custom-button:hover {
  opacity: .9;
}
.custom-button {
  opacity: .65;
}
</style>