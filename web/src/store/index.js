/*
 * Copyright © 2024 Johannes Zangl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

import {onMounted, reactive} from 'vue';
import axios from 'axios';

// Define the store state
let state = reactive({
    dataLoaded: false,
    configs: {},
    wifi: null,
    deviceInfo: null,
});

// Define the actions
const actions = {
    async updateChannel(updatedObject) {
        try {
            const response = await axios.post(import.meta.env.VITE_API_LOCATION + `/api/config`, updatedObject);
            state.configs[updatedObject.channel] = updatedObject;
        } catch (error) {
            console.error("There was an error: ", error);
        }
    },
    async updateWiFi(updatedObject) {
        try {
            const response = axios.post(import.meta.env.VITE_API_LOCATION + '/api/wifi', updatedObject);
            state.wifi = updatedObject;

        } catch (error) {
            console.error("Unable to update wifi: ", error);
        }
    },
    async fetchChannels() {
        try {
            const response = await axios.get(import.meta.env.VITE_API_LOCATION + '/api/config');
            console.log("received data: " + JSON.stringify(response.data));
            state.configs = new Map();
            response.data.reduce(function (map, obj) {
                console.log("Storing value for " + obj.channel)
                map[obj.channel] = obj;
                return map;
            }, state.configs);
        } catch (error) {
            console.error('Error fetching channel configs:', error);
        }
    },
    async fetchDeviceInfo() {
        try {
            const response = await axios.get(import.meta.env.VITE_API_LOCATION + '/api/status');
            state.deviceInfo = response.data;
        } catch (error) {
            console.error('Error fetching device info:', error);
        }
    },
    async fetchWiFi() {
        try {
            const response = await axios.get(import.meta.env.VITE_API_LOCATION + '/api/wifi');
            state.wifi = response.data;
        } catch (error) {
            console.log('Error fetching wifi info:', error);
        }
    }
};

export default {
    state, actions,
    async initialize() {
        await actions.fetchChannels();
        await actions.fetchDeviceInfo();
        await actions.fetchWiFi();
    }
};
