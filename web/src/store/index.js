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

import {reactive} from 'vue';
import {cloneDeep, isEqual} from "lodash";
import {api} from "@/api.js";

// Define the store state
let state = reactive({
    dataLoaded: false,
    /**
     * Our config (in UI)
     */
    configs: {},
    /**
     * Config that is stored on server
     */
    serverConfigs: {},
    currentlySentConfigs: [],

    /**
     * Our current wifi config (in UI)
     */
    wifi: null,
    /**
     * Config that is stored on server
     */
    serverWifi: null,
    /**
     * Whether the wifi config is currently updating
     */
    wifiIsUpdating: false,
    deviceInfo: null,
});


// Define the actions
const actions = {
    async updateChannel(channel) {
        if (state.currentlySentConfigs.includes(channel)) {
            return;
        }
        try {
            state.currentlySentConfigs.push(channel);
            const data = cloneDeep(state.configs[channel]);
            await api.post(`/api/config`, data, {
                timeout: 2000
            });
            state.serverConfigs[channel] = data;
        } catch (error) {
            console.error("There was an error: ", error);
        } finally {
            state.currentlySentConfigs = state.currentlySentConfigs.filter(it => it !== channel);
        }
    },
    async updateWiFi() {
        if (state.wifiIsUpdating) {
            return;
        }
        try {
            state.wifiIsUpdating = true;
            const data = cloneDeep(state.wifi);
            await api.post('/api/wifi', data, {
                timeout: 2000
            });
            state.serverWifi = data;

        } catch (error) {
            console.error("Unable to update wifi: ", error);
        } finally {
            state.wifiIsUpdating = false;
        }
    },
    async fetchChannels() {
        try {
            const response = await api.get('/api/config');
            console.log("received data: " + JSON.stringify(response.data));
            state.configs = {};
            response.data.forEach(obj => {
                state.configs[obj.channel] = obj;
            });
            state.serverConfigs = cloneDeep(state.configs);
        } catch (error) {
            console.error('Error fetching channel configs:', error);
        }
    },
    async fetchDeviceInfo() {
        try {
            const response = await api.get('/api/status');
            state.deviceInfo = response.data;
        } catch (error) {
            console.error('Error fetching device info:', error);
        }
    },
    async fetchWiFi() {
        try {
            const response = await api.get('/api/wifi');
            state.wifi = response.data;
        } catch (error) {
            console.log('Error fetching wifi info:', error);
        }
    }
};

export default {
    state,
    actions,
    async initialize() {
        await actions.fetchChannels();
        await actions.fetchDeviceInfo();
        await actions.fetchWiFi();
    },
    isUpdatingChannel: (channel) => state.currentlySentConfigs.includes(channel),
    shouldSaveChannel: (channel) => !isEqual(state.serverConfigs[channel], state.configs[channel]),
    isUpdatingWifi: () => state.wifiIsUpdating,
    shouldSaveWifi: () => !isEqual(state.serverWifi, state.wifi)
};
