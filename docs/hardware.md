# SYNTEE: Hardware

*← Back to [README](../README.md) | See also: [Pin Mapping](pin-mapping.md) · [Firmware](firmware.md)*

------

## Audio Codec

- **Codec:** 1× AK4619VN (4-in / 4-out, all channels used)
  - 2× stereo ADC (IN 1 L/R + IN 2 L/R — external audio inputs / effects returns)
  - 2× stereo DAC (OUT 1 L/R + OUT 2 L/R — audio outputs)
  - I2S serial audio interface (not TDM — only 4 channels)
  - I2C control interface, address 0x10 (CAD pin low)
  - 48 kHz / 24-bit target
- **Digital Interface:** I2S to Teensy 4.1

### I2S Bus Assignment

- **SAI1 (I2S):** Single AK4619VN codec (all 4 channels)
  - TX_DATA0 (pin 7): Teensy → codec SDIN1 (DAC1 stereo out)
  - TX_DATA1 (pin 32): Teensy → codec SDIN2 (DAC2 stereo out)
  - RX_DATA0 (pin 8): Codec SDOUT1 → Teensy (ADC1 stereo in)
  - RX_DATA1 (pin 6): Codec SDOUT2 → Teensy (ADC2 stereo in)
- **Clock:** Teensy generates MCLK (256fs = 12.288 MHz), BCLK, LRCLK as I2S master
  - BCLK = 48 kHz × 2 channels × 32 bits = 3.072 MHz
  - Codec slaves to Teensy clock

## MCU / DSP Core

- **Teensy 4.1** (ARM Cortex-M7 @ 600 MHz)
  - I2S interface for 4-channel codec (2 stereo pairs in + 2 stereo pairs out)
  - USB host capability for MIDI controllers
  - GPIO for encoders/buttons; UART link to DESPEE display module
  - PJRC Audio Library ecosystem (oscillators, filters, envelopes, effects)
  - Built-in Ethernet (DP83825I PHY, 100 Mbps)
- **DAW connectivity:** Via Ethernet (AES67) — stereo network audio TX + RX

------

## MIDI Interface

- **USB MIDI Host:** USBHost_t36 library, via USB host port on Teensy bottom pads
- **TRS MIDI IN:** 3.5mm TRS Type A, 6N138 optocoupler, 31,250 baud on Serial3 RX (pin 15)
- **TRS MIDI OUT:** 3.5mm TRS Type A, 3.3V current-loop per MMA CA-033, on Serial4 TX (pin 17)
- **Network MIDI 2.0:** Via Ethernet, `_jfa-midi2._udp` service

------

## Power System

### Power Input

- **Dedicated USB-C power input** (5V only, no PD negotiation, power-only — no data lines)
- **PC USB-C** — Teensy native USB for firmware updates and USB MIDI device mode. USB 2.0 High Speed capable — reserved for future USB Audio Class 2 support
- No STUSB4500 needed — SynTee draws under 1A at 5V
- Simple: Power USB-C → polyfuse → 5V rail → Teensy + codec + peripherals

### Power Budget (5V rail)

| Load | Current |
|------|---------|
| Teensy 4.1 + logic | ~200 mA |
| AK4619VN codec | ~20 mA |
| OPA1678 op-amps (×4, input/output stages) | ~60 mA |
| AP2553 USB power switch (quiescent) | ~1 mA |
| 6N138 optocoupler | ~5 mA |
| DESPEE display module (ESP32-S3 + 4.3" LCD + backlight) | ~250–350 mA |
| MAX97220 headphone amp | ~10 mA |
| 3× encoders + 3× buttons | ~5 mA |
| 12× LED-backlit pads | ~25 mA |
| SD card socket | ~50 mA (peak during write) |
| **Total estimate** | **~630–730 mA** |

Within USB-C 5V / 1.5A with headroom.

### Power Distribution

Single 5V rail — no need for isolated domains on a single board. ADP7118 or similar LDO generates clean 3.3V analog supply for the codec. Teensy has its own onboard 3.3V regulator for digital logic.

------

## Key ICs

| Part | Quantity | Notes |
|------|----------|-------|
| Teensy 4.1 | 1 | ARM Cortex-M7, USB host, I2S audio, Ethernet |
| AK4619VN | 1 | 4-in/4-out codec, all channels used (2× stereo in + 2× stereo out) |
| 6N138 | 1 | MIDI IN optocoupler |
| RJ45 MagJack | 1 | Ethernet with integrated magnetics |
| ADP7118 | 1 | 5V → 3.3V_A ultra-low-noise LDO for codec analog supply |
| AP2553 | 1 | USB host VBUS power switch (overcurrent protection, ~500 mA limit) |
| DESPEE module | 1 | External display ([openaudiotools/despee](https://github.com/openaudiotools/despee)); ESP32-S3 + 4.3" 800×480 LCD; 6-pin JST-PH UART |
| MAX97220 | 1 | Headphone amplifier, stereo, drives 3.5mm headphone jack |
| OPA1678 | 4 | Dual op-amp (single-supply rail-to-rail, 3.3V_A). Input buffers + output Sallen-Key filters for 2× stereo pairs |

### Passives & Protection

| Part | Quantity | Notes |
|------|----------|-------|
| 0.1 µF ceramic caps | 20+ | Local decoupling |
| Polyfuse | 1 | USB input protection |
| 1 kΩ resistors | 2 | Audio input series protection (before BAT54 clamp) |
| BAT54 Schottky diodes | 4+ | ESD/overvoltage clamp on audio inputs (to 3.3V_A and GND) |
| 1N4148 signal diode | 1 | MIDI IN optocoupler LED protection |

------

## Target Specifications

### Audio Performance (Target Goals)

- **Sample Rate:** 48 kHz
- **Bit Depth:** 24-bit
- **Latency:** <5 ms (synth engine → DAC)
- **THD+N:** <0.01% (codec-dependent, AK4619VN: 0.001% typ at 1 kHz)
- **Dynamic Range:** >100 dB (codec-dependent)

### Physical (Estimated)

- **Board dimensions:** 140 × 200 mm (main board); additional rear jack board for two rows of back-panel connectors
- **Power consumption:** 5V @ ~680 mA typical
