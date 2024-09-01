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
  <CollapsibleCard class="my-2 servo-card">
    <template #title>
      {{ $t("channel.title", {channel: channel}) }}
    </template>
    <b-form @submit.prevent="submit">
      <b-form-group :label="$t('channel.type.label')">
        <b-form-select v-model="config.type">
          <b-form-select-option value="Disabled">{{ $t("channel.type.disabled") }}</b-form-select-option>
          <b-form-select-option v-if="servo" value="Servo">{{ $t("channel.type.servo") }}</b-form-select-option>
          <b-form-select-option value="SmartButton">{{ $t("channel.type.smartbutton") }}</b-form-select-option>
        </b-form-select>
      </b-form-group>

      <template v-if="config.type === 'Servo'">
        <h4 class="mt-3">{{ $t("channel.position.label-left") }}</h4>
        <b-row>
          <b-col lg="6">
            <b-form-group :label="$t('channel.position.normal')">
              <TimeSlider v-model.number="config.servo.posLeft" :channel="channel"></TimeSlider>
            </b-form-group>
          </b-col>
          <b-col lg="6">
            <b-form-group :label="$t('channel.position.overdraw')">
              <TimeSlider v-model.number="config.servo.posLeftOverdraw" :channel="channel"></TimeSlider>
            </b-form-group>
          </b-col>
        </b-row>

        <h4 class="mt-3">{{ $t("channel.position.label-right") }}</h4>
        <b-row>
          <b-col lg="6">
            <b-form-group :label="$t('channel.position.normal')">
              <TimeSlider v-model.number="config.servo.posRight" :channel="channel"></TimeSlider>
            </b-form-group>
          </b-col>
          <b-col lg="6">
            <b-form-group :label="$t('channel.position.overdraw')">
              <TimeSlider v-model.number="config.servo.posRightOverdraw" :channel="channel"></TimeSlider>
            </b-form-group>
          </b-col>
        </b-row>

        <b-form-group :label="$t('channel.position.overdraw-time')" class="mt-2">
          <b-row>
            <b-col md="9">
              <b-form-input v-model.number="config.servo.overdrawTime"
                            class="form-range" max="2" min="0" step="0.1"
                            type="range"/>
            </b-col>
            <b-col md="3">
              {{ config.servo.overdrawTime }} s
            </b-col>
          </b-row>
        </b-form-group>
      </template>

      <div v-if="config.type === 'SmartButton'">
        <div v-for="(action, index) in config.button.actions" :key="index">
          <h4 class="mt-3">{{ $t("channel.actions.title") }}</h4>
          <b-row>
            <b-col lg="6">
              <b-form-group :label="$t('channel.actions.channel')">
                <b-form-select v-model="action.channel">
                  <b-form-select-option v-for="ch in possibleSwitches()" :key="ch.channel" :value="ch.channel">
                    {{ ch.channel }}
                  </b-form-select-option>
                </b-form-select>
              </b-form-group>
            </b-col>
            <b-col lg="6">
              <b-form-group :label="$t('channel.actions.direction.label')">
                <b-form-select v-model="action.direction">
                  <b-form-select-option value="Left">{{ $t("channel.actions.direction.left") }}</b-form-select-option>
                  <b-form-select-option value="Right">{{ $t("channel.actions.direction.right") }}</b-form-select-option>
                  <b-form-select-option value="Custom">{{
                      $t("channel.actions.direction.custom")
                    }}
                  </b-form-select-option>
                </b-form-select>
              </b-form-group>
            </b-col>
          </b-row>

          <div v-if="action.direction==='Custom'" class="mt-2">
            <b-form-group :label="$t('channel.actions.direction.time')">
              <TimeSlider v-model.number="action.time" :channel="channel"></TimeSlider>
            </b-form-group>
          </div>
          <div class="mt-2">
            <b-button variant="danger" @click="removeAction(channel, index)">{{
                $t("channel.actions.delete")
              }}
            </b-button>
          </div>
        </div>
      </div>

      <div class="d-flex align-items-center gap-2 flex-wrap mt-2">
        <template v-if="config.type==='SmartButton'">
          <b-button variant="success" @click="addAction(channel)">{{ $t("channel.actions.add") }}</b-button>
        </template>
        <template v-if="config.type==='Servo'">
          <b-button v-on:click="requestAction('Left')">{{ $t("channel.test.left") }}</b-button>
          <b-button v-on:click="requestAction('Right')">{{ $t("channel.test.right") }}</b-button>
        </template>
        <b-button :disabled="isUpdating || !shouldSaveChannel" class="ms-auto" type="submit" variant="primary">
          {{ isUpdating ? $t("channel.updating") : $t("channel.update") }}
        </b-button>
      </div>
    </b-form>
  </CollapsibleCard>
</template>

<style lang="scss" scoped>
.deactivated-card {
  .card-header {
    background-color: darkgray;
    color: black;
  }
}

.servo-card {
  .card-header {
    background-color: lightblue;
    color: black;
  }
}

.button-card {
  .card-header {
    background-color: lightyellow;
    color: black;
  }
}
</style>

<script setup>
import axios from 'axios';
import TimeSlider from "@/components/items/ServoTimeSlider.vue";
import CollapsibleCard from "@/components/items/CollapsableCard.vue"
import store from "@/store";
import {computed, watch, watchEffect} from "vue";
import {api} from "@/api.js";

const props = defineProps({
  channel: {
    type: String,
    required: true
  },
  // can be usd as servo
  servo: {
    type: Boolean,
    required: false,
    default: true
  }
});

const submit = () => store.actions.updateChannel(props.channel);
const config = computed(() => store.state.configs[props.channel]);
const otherConfigs = computed(() => store.state.serverConfigs);
const isUpdating = computed(() => store.isUpdatingChannel(props.channel))
const shouldSaveChannel = computed(() => store.shouldSaveChannel(props.channel))

watch(() => config.value.type, () => {
  if (config.value.type === 'Servo' && !config.value.servo) {
    console.log("Updating servo data");
    config.value.servo = {
      posLeft: 1500,
      posRight: 1500,
      posLeftOverdraw: 1500,
      posRightOverdraw: 1500,
      overdrawTime: 0.2
    };
  } else if (config.value.type === 'SmartButton' && !config.value.button) {
    console.log("Updating button data");
    config.value.button = {
      invertedInput: true,
      invertedOutput: true,
      actions: []
    };
  }
});

watchEffect(() => {
  if (config.value.type === 'SmartButton' && config.value.button) {
    config.value.button.actions.forEach(action => {
      if (action.direction === 'Custom' && !action.time) {
        action.time = 1500
      }
    });
  }
});

const possibleSwitches = () => {
  return Object.values(otherConfigs.value).filter(ch => ch.type === "Servo")
};
const addAction = () => {
  config.value.button.actions.push({
    channel: "A1",
    direction: "Left"
  })
};
const removeAction = (index) => {
  config.value.button.actions.splice(index, 1);
};
const requestAction = (direction, time = 0) => {
  const reqData = {
    channel: props.channel,
    direction: direction,
    time: time
  }
  api.post(`/api/channel`, reqData)
};
</script>

<style scoped>
.input-group > * {
  flex: 1;
  min-width: calc(33.333% - 0.5rem);
}

.input-group b-form-input,
.input-group b-form-select {
  width: 100%;
}
</style>
