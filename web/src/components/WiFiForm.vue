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
  <CollapsibleCard class="my-2">
    <template #title>
      {{  $t("wifi.title") }}
    </template>
    <b-form @submit.prevent="sendUpdate(config)">
      <CardSection>

        <b-form-group class="row">
          <label class="col-4" for="wifi-mode">{{ $t('wifi.mode.label') }}</label>
          <div class="col-8">
            <b-form-select id="wifi-mode" v-model="config.mode">
              <b-form-select-option value="Off">{{  $t("wifi.mode.off") }}</b-form-select-option>
              <b-form-select-option value="STA">{{  $t("wifi.mode.sta") }}</b-form-select-option>
              <b-form-select-option value="AP">{{  $t("wifi.mode.ap") }}</b-form-select-option>
            </b-form-select>
          </div>
        </b-form-group>

        <b-form-group class="mr-2" label-for="hostname" :label="$t('wifi.hostname')">
          <b-form-input id="hostname" v-model="config.hostname" type="text"></b-form-input>
        </b-form-group>
      </CardSection>

      <CardSection>
        <h4>{{  $t("wifi.sta.title") }}</h4>
        <b-form-group class="mr-2" label-for="wifi-sta-ssid" :label="$t('wifi.sta.ssid')">
          <b-form-input id="wifi-sta-ssid" v-model="config.sta.ssid" type="text"></b-form-input>
        </b-form-group>
        <b-form-group class="mr-2" label-for="wifi-sta-passphrase" :label="$t('wifi.sta.passphrase')">
          <b-form-input id="wifi-sta-passphrase" v-model="config.sta.passphrase" type="text"></b-form-input>
        </b-form-group>

        <b-form-group class="mr-2" label-for="wifi-sta-method" :label="$t('wifi.sta.method')">
          <b-form-select id="wifi-sta-method" v-model="config.sta.method">
            <b-form-select-option value="dhcp">{{ $t("wifi.sta.method-dhcp") }}</b-form-select-option>
            <b-form-select-option value="static">{{ $t("wifi.sta.method-static") }}</b-form-select-option>
          </b-form-select>
        </b-form-group>
        <div v-if="config.sta.method==='static'">
          <b-form-group class="mr-2" label-for="wifi-sta-static-address" :label="$t('wifi.sta.address')">
            <b-form-input id="wifi-sta-static-address" v-model="config.sta.staticIp.address" type="text" required
                          pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$"></b-form-input>
          </b-form-group>
          <b-form-group class="mr-2" label-for="wifi-sta-static-gateway" :label="$t('wifi.sta.gateway')">
            <b-form-input id="wifi-sta-static-gateway" v-model="config.sta.staticIp.gateway" type="text" required
                          pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$"></b-form-input>
          </b-form-group>
          <b-form-group class="mr-2" label-for="wifi-sta-static-netmask" :label="$t('wifi.sta.netmask')">
            <b-form-input id="wifi-sta-static-netmask" v-model="config.sta.staticIp.netmask" type="text" required
                          pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$"></b-form-input>
          </b-form-group>
        </div>
      </CardSection>

      <CardSection>
        <h4>{{  $t("wifi.ap.title") }}</h4>
        <b-form-group class="mr-2" label-for="wifi-ap-ssid" :label="$t('wifi.ap.ssid')">
          <b-form-input id="wifi-ap-ssid" v-model="config.ap.ssid" type="text"></b-form-input>
        </b-form-group>
        <b-form-group class="mr-2" label-for="wifi-ap-passphrase" :label="$t('wifi.ap.passphrase')">
          <b-form-input id="wifi-ap-passphrase" v-model="config.ap.passphrase" type="text"></b-form-input>
        </b-form-group>
      </CardSection>

      <b-container class="my-3">
        <b-button type="submit" variant="primary">{{$t("wifi.update")}}</b-button>
      </b-container>
    </b-form>

  </CollapsibleCard>
</template>

<script>
import CollapsibleCard from "@/components/items/CollapsableCard.vue";
import CardSection from "@/components/items/CardSection.vue";
import store from "@/store";

export default {
  components: {CardSection, CollapsibleCard},
  setup() {
    return {
      sendUpdate: store.actions.updateWiFi
    }
  },
  data() {
    return {
      config: store.state.wifi
    };
  },
}
</script>
