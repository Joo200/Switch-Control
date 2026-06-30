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

import {layout} from "@/store/capabilities.js";

const storageKeyConfigs = 'demo_configs';
const storageKeyWifi = 'demo_wifi';

const initialConfigs = layout.map(l => ({
    channel: l.channel,
    type: "Disabled",
    servo: {
        posLeft: 1300,
        posRight: 1700,
        posLeftOverdraw: 1250,
        posRightOverdraw: 1750,
        overdrawTime: 1
    },
    button: {
        invertedInput: false,
        invertedOutput: false,
        actionOnPress: []
    }
}));

const initialWifi = {
    mode: "STA",
    hostname: "switch-control-demo",
    sta: {
        ssid: "Target-SSID",
        passphrase: "Target-Passphrase",
        method: "dhcp",
        address: "192.168.1.100",
        gateway: "192.168.1.1",
        netmask: "255.255.255.0",
    },
    ap: {
        ssid: "Switch-Control-Demo",
        passphrase: "Not-Available",
    }
};

const initialStatus = {
    app: {
        title: "Demo App",
        name: "Example Application",
        version: "1.0.0-DUMMY",
        git: "HASH",
        date: "Release Date",
        time: "Release Time",
        "idf-version": "framework version"
    },
    chip: {
        model: "Demo Chip",
        cores: "0",
        revision: "999",
    },
    wifi: {
        mode: "Unknown",
        connected: false
    }
};

function getStored(key, initial) {
    const stored = localStorage.getItem(key);
    return stored ? JSON.parse(stored) : initial;
}

function setStored(key, value) {
    localStorage.setItem(key, JSON.stringify(value));
}

let configs = getStored(storageKeyConfigs, initialConfigs);
let wifi = getStored(storageKeyWifi, initialWifi);

export const mockApi = {
    get(url) {
        console.log(`Mock GET: ${url}`);
        return new Promise((resolve) => {
            setTimeout(() => {
                if (url === '/api/config') {
                    resolve({ data: configs });
                } else if (url === '/api/wifi') {
                    resolve({ data: wifi });
                } else if (url === '/api/status') {
                    resolve({ data: initialStatus });
                }
            }, 200);
        });
    },
    post(url, data) {
        console.log(`Mock POST: ${url}`, data);
        return new Promise((resolve) => {
            setTimeout(() => {
                if (url === '/api/config') {
                    const idx = configs.findIndex(c => c.channel === data.channel);
                    if (idx !== -1) {
                        configs[idx] = data;
                    } else {
                        configs.push(data);
                    }
                    setStored(storageKeyConfigs, configs);
                    resolve({ data: { status: "ok" } });
                } else if (url === '/api/wifi') {
                    wifi = data;
                    setStored(storageKeyWifi, wifi);
                    resolve({ data: { status: "ok" } });
                }
            }, 500);
        });
    }
};
