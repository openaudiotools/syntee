# SYNTEE: Teensy 4.1 Pin Mapping

*← Back to [README](../README.md) | See also: [Hardware](hardware.md) · [Firmware](firmware.md)*

------

## Pin Reference Source

All assignments verified against the official [PJRC Teensy 4.1 reference cards](https://www.pjrc.com/store/teensy41.html) (card11a/card11b rev4) and the PJRC Audio Library source code.

------

## Peripheral Pin Assignments (Fixed)

These pins are consumed by dedicated hardware peripherals and cannot be reassigned.

### SAI1 — I2S Bus (AK4619VN Codec)

| Teensy Pin | Function | Card Label | Direction |
|-----------|----------|------------|-----------|
| **23** | SAI1 MCLK | MCLK1 | Output (master clock) |
| **21** | SAI1 BCLK | BCLK1 | Output (bit clock) |
| **20** | SAI1 LRCLK | LRCLK1 | Output (frame sync) |
| **7** | SAI1 TX_DATA0 | OUT1A | Output (Teensy → codec SDIN) |
| **8** | SAI1 RX_DATA0 | IN1 | Input (codec SDOUT → Teensy) |

BCLK = 48 kHz × 2 × 32 bits = **3.072 MHz**. MCLK = 256×fs = **12.288 MHz**.

**Note:** Pin 23 is also the built-in LED — LED unavailable when audio is active.

### I2C — Codec Control Bus

| Teensy Pin | Function | Bus |
|-----------|----------|-----|
| **18** | SDA | Wire (I2C0) |
| **19** | SCL | Wire (I2C0) |

AK4619VN at address **0x10** (CAD pin low). No mux needed — single codec. External pull-ups: 4.7kΩ to 3.3V.

### Serial3 — MIDI IN (6N138 Output)

| Teensy Pin | Function |
|-----------|----------|
| **15** | RX (MIDI data in) |

MIDI IN is receive-only at 31,250 baud. Pin 15 configured as Serial3 RX.

### Serial4 — MIDI OUT

| Teensy Pin | Function |
|-----------|----------|
| **17** | TX (MIDI data out) |

MIDI OUT is transmit-only at 31,250 baud. 3.3V MIDI current-loop output per MMA CA-033 via 3.5mm TRS Type A jack.

### AK4619VN PDN (Power Down) Control

| Teensy Pin | Function |
|-----------|----------|
| **2** | AK4619VN PDN (active-low) |

GPIO output. Hold LOW during power-up, set HIGH after power rails and MCLK are stable. See architecture.md for reset sequencing.

### USB Host (Bottom Pads)

| Pad | Signal |
|-----|--------|
| +5V | VBUS out |
| D- | USB data minus |
| D+ | USB data plus |
| GND | Ground |

Dedicated USB2 PHY pads on the bottom of the Teensy 4.1. Use `USBHost_t36` library for USB MIDI host.

### Ethernet (Bottom Pads)

| Pad | Signal | Notes |
|-----|--------|-------|
| TX+ | Ethernet transmit positive | Differential pair |
| TX- | Ethernet transmit negative | Differential pair |
| RX+ | Ethernet receive positive | Differential pair |
| RX- | Ethernet receive negative | Differential pair |
| LED | Activity LED (optional) | Active-low |
| GND | Ground | Shield/return |

Routed from Teensy bottom pads directly to RJ45 MagJack with integrated magnetics. No external coupling capacitors — Teensy PHY has on-chip AC coupling, MagJack has integrated isolation. Use short, 100Ω impedance-controlled differential traces.

------

## Peripheral Pin Summary

| Pin | Peripheral |
|-----|-----------|
| 2 | AK4619VN PDN (GPIO) |
| 7 | SAI1 TX_DATA0 |
| 8 | SAI1 RX_DATA0 |
| 15 | Serial3 RX (MIDI IN) |
| 17 | Serial4 TX (MIDI OUT) |
| 18 | Wire SDA |
| 19 | Wire SCL |
| 20 | SAI1 LRCLK |
| 21 | SAI1 BCLK |
| 23 | SAI1 MCLK |

**Total edge pins consumed by peripherals: 10** (plus bottom pads for USB Host, Ethernet).

------

## GPIO Budget

Total Teensy 4.1 edge pins: **42** (pins 0–41)
Consumed by peripherals: **10** (SAI1 ×5, I2C ×2, Serial3 RX ×1, Serial4 TX ×1, AK4619VN PDN ×1)
Available for GPIO: **32**

### GPIO Requirements (Preliminary)

| Function | Pins needed | Notes |
|----------|------------|-------|
| Encoder 1 | **3** | A, B, push switch |
| Encoder 2 | **3** | A, B, push switch |
| Display interface | **TBD** | SPI or I2C depending on display choice |
| Status LEDs | **1–2** | Power, MIDI activity |
| **Total (minimum)** | **~8** | |

**Remaining spare:** 24+ pins — ample headroom for additional controls, indicators, or expansion.

------

## GPIO Pin Assignments (Preliminary)

| Teensy Pin | Assignment | Notes |
|-----------|-----------|-------|
| **0** | *(spare / future Serial1 RX)* | Available for display UART |
| **1** | *(spare / future Serial1 TX)* | Available for display UART |
| **2** | AK4619VN PDN | Codec power-down control (active-low) |
| **3–6** | *(spare)* | Available for encoders, buttons, LEDs |
| **9–14** | *(spare)* | SPI0 available on 10–13 if needed for display |
| **16** | *(spare)* | |
| **22–41** | *(spare)* | Many interrupt-capable pins available for encoders |

Pin assignments will be finalized when the control interface design is decided.

------

## Conflict Warnings

1. **Serial2 (pins 7/8) is BLOCKED** — these pins are SAI1 audio data lines
2. **Pin 23 = LED** — built-in LED unavailable during audio playback

------

*Pin assignments are preliminary and will be refined as the control interface is designed.*
