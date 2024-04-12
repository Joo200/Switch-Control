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
  <b-card no-body :collapsible="true" @collapse="isCollapsed = true" @expand="isCollapsed = false">
    <template #header>
      <div @click="toggleCollapse" style="cursor: pointer;">
        <b-card-title class="my-2 align-self-center">
          <i-tabler-arrow-badge-down-filled v-if="isCollapsed" class="me-2"/>
          <i-tabler-arrow-badge-up-filled v-if="!isCollapsed" class="me-2"/>
          <slot name="title">Card title</slot>
        </b-card-title>
      </div>
    </template>
    <b-card-body v-if="!isCollapsed" >
      <slot>This is the content of the card. It will collapse and expand when the title is clicked.</slot>
    </b-card-body>
  </b-card>
</template>

<script>
// import MaterialSymbolsArrowDropDownCircle from '~icons/material-symbols/arrow-drop-down-circle';

export default {
  name: 'CollapsibleCard',
  data() {
    return {
      isCollapsed: true
    };
  },
  methods: {
    toggleCollapse() {
      this.isCollapsed = !this.isCollapsed;
    }
  }
};
</script>
