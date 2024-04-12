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
      {{ $t("channel.title", {channel: channel}) }}
    </template>
    <b-form @submit.prevent="updateConfig(config)">
      <b-form-group :label="$t('channel.type.label')">
        <b-form-select v-model="config.type" @change="handleTypeChange">
          <b-form-select-option value="Disabled">{{  $t("channel.type.disabled") }}</b-form-select-option>
          <b-form-select-option v-if="servo" value="Servo">{{  $t("channel.type.servo") }}</b-form-select-option>
          <b-form-select-option value="SmartButton">{{  $t("channel.type.smartbutton") }}</b-form-select-option>
        </b-form-select>
      </b-form-group>

      <b-container v-if="config.type === 'Servo'">
        <CardSection>
          <h4>{{  $t("channel.position.label-left") }}</h4>
          <TimeSlider :channel="channel" :label="$t('channel.position.normal')" v-model.number="config.servo.posLeft"></TimeSlider>
          <TimeSlider :channel="channel" :label="$t('channel.position.overdraw')" v-model.number="config.servo.posLeftOverdraw"></TimeSlider>
        </CardSection>

        <CardSection>
          <h4>{{  $t("channel.position.label-right") }}</h4>
          <TimeSlider :channel="channel" :label="$t('channel.position.normal')" v-model.number="config.servo.posRight"></TimeSlider>
          <TimeSlider :channel="channel" :label="$t('channel.position.overdraw')"
                      v-model.number="config.servo.posRightOverdraw"></TimeSlider>
        </CardSection>

        <CardSection>
          <b-row>
            <b-col cols="12" md="2" sm="3">
              <label class="col-form-label">
                {{ $t("channel.position.overdraw-time")}}
              </label>
            </b-col>
            <b-col>
              <b-form-group>
                <b-form-input type="range" class="form-range" v-model.number="config.servo.overdrawTime" min="0" max="2"
                              step="0.1"></b-form-input>
              </b-form-group>
            </b-col>
            <b-col cols="12" md="3">{{ config.servo.overdrawTime }} s</b-col>
          </b-row>
        </CardSection>
      </b-container>

      <div v-if="config.type === 'SmartButton'">
        <h4>{{ $t("channel.actions.title")}}</h4>
        <div v-for="(action, index) in config.button.actions" :key="index">
          <CardSection>
            <b-form-group :label="$t('channel.actions.channel')">
              <b-form-select v-model="action.channel">
                <b-form-select-option v-for="ch in possibleSwitches()" :key="ch.channel" :value="ch.channel">
                  {{ ch.channel }}
                </b-form-select-option>
              </b-form-select>
            </b-form-group>

            <b-row>
              <b-col>
                <h5>{{ $t("channel.actions.direction.label")}}</h5>
              </b-col>
              <b-col>
                <b-form-select v-model="action.direction">
                  <b-form-select-option value="Left">{{$t("channel.actions.direction.left")}}</b-form-select-option>
                  <b-form-select-option value="Right">{{$t("channel.actions.direction.right")}}</b-form-select-option>
                  <b-form-select-option value="Custom">{{$t("channel.actions.direction.custom")}}</b-form-select-option>
                </b-form-select>
              </b-col>
            </b-row>

            <b-row v-if="action.direction==='Custom'">
              <b-col cols="12" md="3"><h5>{{$t("channel.actions.direction.time")}}</h5></b-col>
              <b-col>
                <b-form-input type="range" class="form-range" v-model.number="action.time" min="800"
                              max="2200"></b-form-input>
              </b-col>
              <b-col cols="12" md="3">
                <b-form-input type="number" class="form-text" v-model.number="action.time" min="800"
                              max="2200"></b-form-input>
              </b-col>
            </b-row>
            <b-button @click="removeAction(channel, index)" variant="danger">{{$t("channel.actions.delete")}}</b-button>
          </CardSection>
        </div>
        <CardSection>
          <b-button @click="addAction(channel)" variant="success">{{$t("channel.actions.add")}}</b-button>
        </CardSection>
      </div>

      <b-container class="my-3">
        <b-button type="submit" variant="primary">{{$t("channel.update")}}</b-button>
      </b-container>
    </b-form>
    <b-button-group v-if="config.type==='Servo'">
      <b-container class="my-2">
        <b-button v-on:click="requestAction('Left')">{{$t("channel.test.left")}}</b-button>
        <b-button v-on:click="requestAction('Right')">{{$t("channel.test.right")}}</b-button>
      </b-container>
    </b-button-group>
  </CollapsibleCard>
</template>

<script>
import axios from 'axios';
import TimeSlider from "@/components/items/ServoTimeSlider.vue";
import CollapsibleCard from "@/components/items/CollapsableCard.vue"
import CardSection from "@/components/items/CardSection.vue";
import store from "@/store";

export default {
  components: {CardSection, CollapsibleCard, TimeSlider},
  props: {
    channel: {
      type: String,
      required: true
    },
    servo: {
      type: Boolean,
      required: false,
      default: true
    }
  },
  setup() {
    return {
      updateConfig: store.actions.updateChannel
    }
  },
  data() {
    return {
      config: store.state.configs[this.channel],
      otherConfigs: store.state.configs
    }
  },
  methods: {
    handleTypeChange() {
      if (this.config.type === 'Servo' && this.config.servo == null) {
        console.log("Updating servo data");
        this.config.servo = {
          posLeft: 1500,
          posRight: 1500,
          posLeftOverdraw: 1500,
          posRightOverdraw: 1500,
          overdrawTime: 0.2
        };
      } else if (this.config.type === 'SmartButton' && this.config.button == null) {
        console.log("Updating button data");
        this.config.button = {
          invertedInput: true,
          invertedOutput: true,
          actions: []
        };
      }
    },
    possibleSwitches() {
      return Object.values(this.otherConfigs).filter(ch => ch.type === "Servo")
    },
    addAction() {
      this.config.button.actions.push({
        channel: "A1",
        direction: "Left"
      })
    },
    removeAction(index) {
      this.config.button.actions.splice(index, 1);
    },
    requestAction(direction, time = 0) {
      const reqData = {
        channel: this.channel,
        direction: direction,
        time: time
      }
      axios.post(`/api/channel`, reqData)
    }
  }
};
</script>

<style>
.input-group > * {
  flex: 1;
  min-width: calc(33.333% - 0.5rem);
}

.input-group b-form-input,
.input-group b-form-select {
  width: 100%;
}
</style>
