// ==UserScript==
// @name         Advent of Code Arduino notifier
// @namespace    https://github.com/sanraith/aoc-arduino-stars
// @version      2024-10-11
// @description  Notify my arduino display to re-check my progress when I post a solution on adventofcode.com.
// @author       sanraith
// @match        https://adventofcode.com/*/day/*/answer
// @icon         https://www.google.com/s2/favicons?sz=64&domain=adventofcode.com
// @grant        GM_xmlhttpRequest
// @connect      192.168.0.16
// ==/UserScript==

(function () {
    'use strict';

    console.log("[Arduino notifier] Waiting for page load...");
    window.addEventListener('load', function () {
        console.log("[Arduino notifier] Connecting to arduino...");
        GM_xmlhttpRequest({
            method: "GET",
            url: "http://192.168.0.16/update",
            onload: data => console.log(data),
            onerror: error => console.error('[Arduino notifier] Error: ', error)
        });
    }, false);
})();