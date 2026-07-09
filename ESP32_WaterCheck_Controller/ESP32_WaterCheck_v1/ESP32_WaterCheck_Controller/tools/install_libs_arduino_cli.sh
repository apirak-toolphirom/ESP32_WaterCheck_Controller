#!/usr/bin/env bash
set -e

echo "Installing ESP32 board package and required libraries..."
arduino-cli config init --overwrite || true
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json || true
arduino-cli core update-index
arduino-cli core install esp32:esp32
arduino-cli lib update-index
arduino-cli lib install "WiFiManager"
arduino-cli lib install "PubSubClient"
arduino-cli lib install "ArduinoJson"
arduino-cli lib install "LiquidCrystal I2C"
echo "Done. Select board: esp32:esp32:esp32"
