# SYNTEE: Hardware

*← Back to [README](../README.md) | See also: [Pin Mapping](pin-mapping.md) · [Firmware](firmware.md)*

------

## Audio Codec

- **Codec:** 1× AK4619VN (4-in / 4-out, using 2-in / 2-out for stereo)
  - Stereo ADC (external audio input / effects return)
  - Stereo DAC (synthesizer output)
  - I2S serial audio interface (not TDM — only 4 channels)
  - I2C control interface, address 0x10 (CAD pin low)
  - 48 kHz / 24-bit target
- **Digital Interface:** I2S to Teensy 4.1

### I2S Bus Assignment

- **SAI1 (I2S):** Single AK4619VN codec
  - TX_DATA0 (pin 7): Teensy → codec SDIN (stereo DAC)
  - RX_DATA0 (pin 8): Codec SDOUT → Teensy (stereo ADC)
- **Clock:** Teensy generates MCLK (256fs = 12.288 MHz), BCLK, LRCLK as I2S master
  - BCLK = 48 kHz × 2 channels × 32 bits = 3.072 MHz
  - Codec slaves to Teensy clock

## MCU / DSP Core

- **Teensy 4.1** (ARM Cortex-M7 @ 600 MHz)
  - I2S interface for stereo codec
  - USB host capability for MIDI controllers
  - GPIO for encoders/buttons/display
  - PJRC Audio Library ecosystem (oscillators, filters, envelopes, effects)
  - Built-in Ethernet (DP83825I PHY, 100 Mbps)
- **DAW connectivity:** Via Ethernet (AES67) — stereo network audio TX

------

## MIDI Interface

- **USB MIDI Host:** USBHost_t36 library, via USB host port on Teensy bottom pads
- **TRS MIDI IN:** 3.5mm TRS Type A, 6N138 optocoupler, 31,250 baud on Serial3 RX (pin 15)
- **TRS MIDI OUT:** 3.5mm TRS Type A, 3.3V current-loop per MMA CA-033, on Serial4 TX (pin 17)
- **Network MIDI 2.0:** Via Ethernet, `_jfa-midi2._udp` service

------

## Power System

### Power Input

- **USB 5V** — standard USB-B or USB-C connector (5V only, no PD negotiation)
- No STUSB4500 needed — SynTee draws well under 500 mA
- Simple: USB connector → polyfuse → 5V rail → Teensy + codec + peripherals

### Power Budget (5V rail)

| Load | Current |
|------|---------|
| Teensy 4.1 + logic | ~200 mA |
| AK4619VN codec | ~20 mA |
| OPA1678 op-amps (input/output stages) | ~30 mA |
| AP2553 USB power switch (quiescent) | ~1 mA |
| 6N138 optocoupler | ~5 mA |
| Display (TBD) | ~50–100 mA |
| Encoders/buttons/LEDs | ~20 mA |
| **Total estimate** | **~325–375 mA** |

Well within USB 5V / 500 mA. A USB-C connector at 5V / 1.5A provides ample headroom.

### Power Distribution

Single 5V rail — no need for isolated domains on a single board. ADP7118 or similar LDO generates clean 3.3V analog supply for the codec. Teensy has its own onboard 3.3V regulator for digital logic.

------

## Key ICs

| Part | Quantity | Notes |
|------|----------|-------|
| Teensy 4.1 | 1 | ARM Cortex-M7, USB host, I2S audio, Ethernet |
| AK4619VN | 1 | 4-in/4-out codec, using stereo in + stereo out |
| 6N138 | 1 | MIDI IN optocoupler |
| RJ45 MagJack | 1 | Ethernet with integrated magnetics |
| ADP7118 | 1 | 5V → 3.3V_A ultra-low-noise LDO for codec analog supply |
| AP2553 | 1 | USB host VBUS power switch (overcurrent protection, ~500 mA limit) |
| OPA1678 | 2 | Dual op-amp (single-supply rail-to-rail, 3.3V_A). Input buffers + output Sallen-Key filter |

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

- **Board dimensions:** TBD (single PCB, compact)
- **Power consumption:** 5V @ ~350 mA typical
