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
  <b-container>
    <b-row class="align-items-center mb-2">
      <b-col cols="12" md="2" sm="3">
        <label class="col-form-label">
          {{ label }}
        </label>
      </b-col>
      <b-col cols="12" md="7" sm="9" class="d-flex align-items-center">
        <b-form-input
            type="range"
            class="form-range w-100 mb-2 mb-sm-0"
            :min="min"
            :max="max"
            step="5"
            v-model="internalValue"
            @input="updateValue"
        />
      </b-col>
      <b-col cols="auto" class="d-flex align-items-center mb-2 mb-sm-0">
        <b-form-input
            type="number"
            v-model="internalValue"
            @input="updateValue"
            class="form-control"
            :min="min"
            :max="max"
            step="5"
            style="width: 100px;"
        />
        <b-button-group class="d-lg-none mx-2">
          <b-button
              class="btn btn-primary"
              @click="decreaseValue"
          >&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;
          </b-button>
          <b-button
              class="btn btn-primary"
              @click="increaseValue"
          >&nbsp;&nbsp;&nbsp;+&nbsp;&nbsp;&nbsp;
          </b-button>
        </b-button-group>
      </b-col>
    </b-row>
  </b-container>
</template>

<script>
import axios from "axios";

export default {
  name: 'TimeSlider',
  props: {
    channel: {
      type: String,
      required: true,
    },
    label: {
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
  },
  data() {
    return {
      internalValue: this.modelValue,
    };
  },
  watch: {
    modelValue(newValue) {
      this.internalValue = newValue;
    },
  },
  methods: {
    updateValue() {
      this.$emit('update:modelValue', this.internalValue);

      const data = {
        channel: this.channel,
        direction: "Custom",
        time: parseInt(this.internalValue)
      }
      axios.post(import.meta.env.VITE_API_LOCATION + `/api/channel`, data)
          .then(response => {
            console.log('Temp data sent updated:');
          })
          .catch(error => {
            console.error('Error updating temp data:', error);
          });
    },
    decreaseValue() {
      if (this.internalValue > this.min) {
        this.internalValue -= 5;
        this.updateValue();
      }
    },
    increaseValue() {
      if (this.internalValue < this.max) {
        this.internalValue += 5;
        this.updateValue();
      }
    },
  },
};
</script>
