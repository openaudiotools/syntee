#include <Arduino.h>
#include <Audio.h>

// SynTee — Virtual Sound Module / Synthesizer
// Teensy 4.1 + AK4619VN codec + QNEthernet
//
// Init sequence (planned):
//   1. Configure I2S output (AudioOutputI2S)
//   2. Initialize AK4619VN codec via I2C (Wire, address 0x10)
//   3. Initialize MIDI inputs (USBHost, Serial3 RX)
//   4. Initialize QNEthernet (DHCP, mDNS, PTP)
//   5. Start synth engine (audio objects patched in setup)
//   6. Main loop: poll MIDI, update UI, service network

void setup() {
    Serial.begin(115200);
    AudioMemory(20);

    // TODO: Initialize audio objects, codec, MIDI, network
}

void loop() {
    // TODO: Poll MIDI, update parameters, service network
}
