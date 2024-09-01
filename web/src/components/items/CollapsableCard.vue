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
  <b-card :collapsible="true" class="device-card" no-body @collapse="isCollapsed = true" @expand="isCollapsed = false">
    <template #header>
      <div style="cursor: pointer;" @click="toggleCollapse">
        <b-card-title class="my-2 align-self-center">
          <i-tabler-arrow-badge-down-filled v-if="isCollapsed" class="me-2"/>
          <i-tabler-arrow-badge-up-filled v-if="!isCollapsed" class="me-2"/>
          <slot name="title">Card title</slot>
        </b-card-title>
      </div>
    </template>
    <b-card-body v-if="!isCollapsed">
      <slot>

      </slot>
    </b-card-body>
  </b-card>
</template>

<style lang="scss" scoped>
.device-card {
  background-color: white;
  color: black;

  .card-header {
    background-color: blue;
    color: white;
  }
}
</style>

<script setup>
import {ref} from "vue";

const isCollapsed = ref(true);
const toggleCollapse = () => isCollapsed.value = !isCollapsed.value;
</script>
