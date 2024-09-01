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
  <CollapsibleCard class="my-2 wifi-card">
    <template #title>
      {{ $t("wifi.title") }}
    </template>
    <b-form @submit.prevent="submit">

      <b-form-group :label="$t('wifi.mode.label')">
        <b-form-select id="wifi-mode" v-model="config.mode">
          <b-form-select-option value="Off">{{ $t("wifi.mode.off") }}</b-form-select-option>
          <b-form-select-option value="STA">{{ $t("wifi.mode.sta") }}</b-form-select-option>
          <b-form-select-option value="AP">{{ $t("wifi.mode.ap") }}</b-form-select-option>
        </b-form-select>
      </b-form-group>

      <b-form-group :label="$t('wifi.hostname')" label-for="hostname">
        <b-form-input id="hostname" v-model="config.hostname" type="text"></b-form-input>
      </b-form-group>

      <h4 class="mt-3">{{ $t("wifi.sta.title") }}</h4>
      <b-row>
        <b-col lg="6">
          <b-form-group :label="$t('wifi.sta.ssid')" label-for="wifi-sta-ssid">
            <b-form-input id="wifi-sta-ssid" v-model="config.sta.ssid" type="text"></b-form-input>
          </b-form-group>
        </b-col>
        <b-col lg="6">
          <b-form-group :label="$t('wifi.sta.passphrase')" label-for="wifi-sta-passphrase">
            <b-form-input id="wifi-sta-passphrase" v-model="config.sta.passphrase" type="text"></b-form-input>
          </b-form-group>
        </b-col>
      </b-row>

      <b-form-group :label="$t('wifi.sta.method')" label-for="wifi-sta-method">
        <b-form-select id="wifi-sta-method" v-model="config.sta.method">
          <b-form-select-option value="dhcp">{{ $t("wifi.sta.method-dhcp") }}</b-form-select-option>
          <b-form-select-option value="static">{{ $t("wifi.sta.method-static") }}</b-form-select-option>
        </b-form-select>
      </b-form-group>
      <div v-if="config.sta.method==='static'">
        <b-form-group :label="$t('wifi.sta.address')" label-for="wifi-sta-static-address">
          <b-form-input id="wifi-sta-static-address" v-model="config.sta.staticIp.address"
                        pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$" required
                        type="text"></b-form-input>
        </b-form-group>
        <b-form-group :label="$t('wifi.sta.gateway')" label-for="wifi-sta-static-gateway">
          <b-form-input id="wifi-sta-static-gateway" v-model="config.sta.staticIp.gateway"
                        pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$" required
                        type="text"></b-form-input>
        </b-form-group>
        <b-form-group :label="$t('wifi.sta.netmask')" label-for="wifi-sta-static-netmask">
          <b-form-input id="wifi-sta-static-netmask" v-model="config.sta.staticIp.netmask"
                        pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$" required
                        type="text"></b-form-input>
        </b-form-group>
      </div>

      <h4 class="mt-3">{{ $t("wifi.ap.title") }}</h4>
      <b-form-group :label="$t('wifi.ap.ssid')" class="mr-2" label-for="wifi-ap-ssid">
        <b-form-input id="wifi-ap-ssid" v-model="config.ap.ssid" type="text"></b-form-input>
      </b-form-group>
      <b-form-group :label="$t('wifi.ap.passphrase')" class="mr-2" label-for="wifi-ap-passphrase">
        <b-form-input id="wifi-ap-passphrase" v-model="config.ap.passphrase" type="text"></b-form-input>
      </b-form-group>

      <div class="mt-2">
        <b-button :disabled="isUpdating || !shouldSaveWifi" class="ms-auto" type="submit" variant="primary">
          {{ isUpdating ? $t("wifi.updating") : $t("wifi.update") }}
        </b-button>
      </div>
    </b-form>

  </CollapsibleCard>
</template>

<style lang="scss" scoped>
.wifi-card {
  .card-header {
    background-color: darkgray;
    color: black;
  }
}
</style>

<script setup>
import CollapsibleCard from "@/components/items/CollapsableCard.vue";
import store from "@/store";
import {computed} from "vue";
import {BRow} from "bootstrap-vue-next";

const submit = () => store.actions.updateWiFi();

const config = computed(() => store.state.wifi);
const isUpdating = computed(() => store.isUpdatingWifi());
const shouldSaveWifi = computed(() => store.shouldSaveWifi());

</script>
