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

function ChannelCapabilities(channel, servo) {
    this.channel = channel;
    this.servo = servo;
}

export const layout = [
    new ChannelCapabilities("A1", true),
    new ChannelCapabilities("A2", true),
    new ChannelCapabilities("A3", true),
    new ChannelCapabilities("A4", true),
    new ChannelCapabilities("A5", true),
    new ChannelCapabilities("A6", true),
    new ChannelCapabilities("A7", true),
    new ChannelCapabilities("A8", true),
    new ChannelCapabilities("B1", false),
    new ChannelCapabilities("B2", false),
    new ChannelCapabilities("B3", false),
    new ChannelCapabilities("B4", false),
    new ChannelCapabilities("B5", false),
    new ChannelCapabilities("B6", false),
    new ChannelCapabilities("B7", false),
    new ChannelCapabilities("B8", false)
]
