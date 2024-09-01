<!--
  - Copyright © 2024 Johannes Zangl
  -
  - Permission is hereby granted, free of charge, to any person obtaining a copy of this software
  - and associated documentation files (the “Software”), to deal in the Software without
  - restriction, including without limitation the rights to use, copy, modify, merge, publish,
  - distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
  - Software is furnished to do so, subject to the following conditions:
  -
  - The above copyright notice and this permission notice shall be included in all copies or
  - substantial portions of the Software.
  -
  - THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
  - BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  - NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  - DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  - FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  -->

<template>
  <div class="d-flex align-items-center mb-2 gap-2">
    <b-form-input
        v-model="internalValue"
        :max="max"
        :min="min"
        class="form-range flex-grow-1 w-auto"
        step="5"
        type="range"
    />
    <div class="flex-grow-0">
      <b-input-group>
        <b-button
            class="btn btn-primary"
            @click="decreaseValue"
        >
          -
        </b-button>
        <input v-model="internalValue"
               :style="{'width': '5em'}"
               class="form-control"
               pattern="[0-9]{3,4}"
               type="text"
        />
        <b-button
            class="btn btn-primary"
            @click="increaseValue"
        >
          +
        </b-button>
      </b-input-group>
    </div>
  </div>
</template>

<script setup>
import {computed, watch} from "vue";
import {api} from "@/api.js";

const props = defineProps({
  channel: {
    type: String,
    required: true,
  },
  modelValue: {
    type: Number,
    required: true,
  },
  min: {
    type: Number,
    default: 800,
  },
  max: {
    type: Number,
    default: 2200,
  },
});

const emit = defineEmits(['update:modelValue']);

const internalValue = computed({
  get: () => props.modelValue,
  set: (value) => emit('update:modelValue', value)
});

const decreaseValue = () => {
  if (internalValue.value > props.min) {
    internalValue.value -= 5;
  }
}

const increaseValue = () => {
  if (internalValue.value < props.max) {
    internalValue.value += 5;
  }
}

watch(() => internalValue.value, (newValue, oldValue) => {
  if (newValue !== oldValue) {
    const data = {
      channel: props.channel,
      direction: "Custom",
      time: newValue
    }
    api.post(`/api/channel`, data)
        .then(response => {
          console.log('Temp data sent updated:');
        })
        .catch(error => {
          console.error('Error updating temp data:', error);
        });
  }
});

</script>
