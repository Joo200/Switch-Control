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
  <CollapsibleCard class="my-2 device-card">
    <template #title>
      {{ $t("devInfo.title") }}
    </template>
    <LanguageSwitcher/>

    <table class="table">
      <tr>
        <th>{{ $t("devInfo.app.title") }}</th>
      </tr>
      <tr>
        <td>{{ $t("devInfo.app.name") }}</td>
        <td>{{ deviceInfo.app.name }}</td>
      </tr>
      <tr>
        <td>{{ $t("devInfo.app.version") }}</td>
        <td>{{ deviceInfo.app.version }}</td>
      </tr>
      <tr>
        <td>{{ $t("devInfo.app.compile_time") }}</td>
        <td>{{ deviceInfo.app.date }} {{ deviceInfo.app.time }}</td>
      </tr>
      <tr>
        <td>{{ $t("devInfo.app.framework") }}</td>
        <td>esp-idf {{ 'deviceInfo.app.idf-version' }}</td>
      </tr>
      <tr>
        <th>{{ $t("devInfo.chip.title") }}</th>
      </tr>
      <tr>
        <td>{{ $t("devInfo.chip.model") }}</td>
        <td>{{ deviceInfo.chip.model }}</td>
      </tr>
      <tr>
        <td>{{ $t("devInfo.chip.cores") }}</td>
        <td>{{ deviceInfo.chip.cores }}</td>
      </tr>
      <tr>
        <td>{{ $t("devInfo.chip.revision") }}</td>
        <td>{{ deviceInfo.chip.revision }}</td>
      </tr>

      <tr>
        <th>{{ $t("devInfo.wifi.title") }}</th>
      </tr>
      <tr>
        <td>{{ $t("devInfo.wifi.mode") }}</td>
        <td>{{ deviceInfo.wifi.mode }}</td>
      </tr>
      <tr v-if="deviceInfo.wifi.connected">
        <td>{{ $t("devInfo.wifi.status") }}</td>
        <td>{{ deviceInfo.wifi.connected }}</td>
      </tr>
    </table>
  </CollapsibleCard>
</template>

<style lang="scss" scoped>
.device-card {
  .card-header {
    background-color: darkgray;
    color: black;
  }
}
</style>

<script setup>
import CollapsibleCard from "@/components/items/CollapsableCard.vue";
import LanguageSwitcher from "@/components/items/LanguageSwitcher.vue";
import store from "@/store";
import {computed} from "vue";

const deviceInfo = computed(() => store.state.deviceInfo);
</script>