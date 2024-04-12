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
  <b-container class="my-1">
    <h1>{{  $t("title") }}</h1>

    <b-container class="my-3">
      <DeviceInfo v-if="deviceInfo"/>
      <WiFiForm v-if="wifi"/>
    </b-container>
    <b-container class="my-3" v-for="channel in ChannelLayout()" :key="channel">
      <ChannelForm v-if="configs[channel.channel]" :channel="channel.channel" :servo="channel.servo"/>
    </b-container>
  </b-container>
</template>

<script>
import {layout} from "@/store/capabilities.js";
import ChannelForm from "@/components/ChannelForm.vue";
import DeviceInfo from "@/components/DeviceInfo.vue";
import WiFiForm from "@/components/WiFiForm.vue";
import store from "@/store";

export default {
  components: {
    WiFiForm, ChannelForm, DeviceInfo
  },
  async setup() {
    await store.initialize()
    return {
      configs: store.state.configs,
      wifi: store.state.wifi,
      deviceInfo: store.state.deviceInfo
    }
  },
  methods: {
    ChannelLayout() {
      return layout
    },
  },
};
</script>