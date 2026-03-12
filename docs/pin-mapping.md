# SYNTEE: Teensy 4.1 Pin Mapping

*← Back to [README](../README.md) | See also: [Hardware](hardware.md) · [Firmware](firmware.md)*

------

## Teensy Mounting

The Teensy 4.1 is mounted via **socketed headers** — male pin headers soldered to the Teensy, inserted into female socket headers on the main board. This covers all **edge pins (0–41)** and the **bottom pads** (USB Host D+/D−, Ethernet TX±/RX±). Bottom pads are soldered to short pin headers on the Teensy underside and connect through matching sockets on the main board. This allows the Teensy to be removed for debugging or replacement without desoldering.

**Impedance note:** The header/socket interface introduces a small impedance discontinuity. This is acceptable for Ethernet (100 Mbps, short traces to MagJack) and USB Host (Full Speed 12 Mbps for MIDI controllers). For PC USB-C (routed directly from Teensy's onboard connector, not through headers), maintain 90Ω differential traces on the main board to preserve USB 2.0 High Speed signal integrity for future USB Audio support.

------

## Pin Reference Source

All assignments verified against the official [PJRC Teensy 4.1 reference cards](https://www.pjrc.com/store/teensy41.html) (card11a/card11b rev4) and the PJRC Audio Library source code.

------

## Peripheral Pin Assignments (Fixed)

These pins are consumed by dedicated hardware peripherals and cannot be reassigned.

### SAI1 — I2S Bus (AK4619VN Codec, 4 channels)

| Teensy Pin | Function | Card Label | Direction |
|-----------|----------|------------|-----------|
| **23** | SAI1 MCLK | MCLK1 | Output (master clock) |
| **21** | SAI1 BCLK | BCLK1 | Output (bit clock) |
| **20** | SAI1 LRCLK | LRCLK1 | Output (frame sync) |
| **7** | SAI1 TX_DATA0 | OUT1A | Output (Teensy → codec SDIN1, DAC1 L/R) |
| **32** | SAI1 TX_DATA1 | OUT1B | Output (Teensy → codec SDIN2, DAC2 L/R) |
| **8** | SAI1 RX_DATA0 | IN1 | Input (codec SDOUT1 → Teensy, ADC1 L/R) |
| **6** | SAI1 RX_DATA1 | IN2 | Input (codec SDOUT2 → Teensy, ADC2 L/R) |

BCLK = 48 kHz × 2 × 32 bits = **3.072 MHz**. MCLK = 256×fs = **12.288 MHz**. I2S mode with 2 data lines per direction for 4 channels total.

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

### Serial1 — DESPEE Display Module

| Teensy Pin | Function |
|-----------|----------|
| **0** | RX (DESPEE TX → Teensy) |
| **1** | TX (Teensy → DESPEE RX) |

Serial1 UART at 921600 baud. Binary widget commands (COBS-encoded, CRC16) to DESPEE; touch coordinates from DESPEE.

### DESPEE Boot Control

| Teensy Pin | Function |
|-----------|----------|
| **22** | ESP32_EN (active-high enable; pull-up on DESPEE module) |
| **37** | ESP32_GPIO0 (boot mode: LOW = UART bootloader for firmware update) |

GPIO outputs for controlling DESPEE module reset and firmware flashing (used by esp-serial-flasher for SD card updates).

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
| 0 | Serial1 RX (DESPEE UART) |
| 1 | Serial1 TX (DESPEE UART) |
| 2 | AK4619VN PDN (GPIO) |
| 6 | SAI1 RX_DATA1 (ADC2) |
| 7 | SAI1 TX_DATA0 (DAC1) |
| 8 | SAI1 RX_DATA0 (ADC1) |
| 15 | Serial3 RX (MIDI IN) |
| 17 | Serial4 TX (MIDI OUT) |
| 18 | Wire SDA |
| 19 | Wire SCL |
| 20 | SAI1 LRCLK |
| 21 | SAI1 BCLK |
| 22 | DESPEE ESP32_EN (GPIO) |
| 23 | SAI1 MCLK |
| 32 | SAI1 TX_DATA1 (DAC2) |
| 37 | DESPEE ESP32_GPIO0 (GPIO) |

**Total edge pins consumed by peripherals: 16** (plus bottom pads for USB Host, Ethernet).

------

## GPIO Budget

Total Teensy 4.1 edge pins: **42** (pins 0–41)
Consumed by peripherals: **16** (SAI1 ×7, I2C ×2, Serial1 ×2, Serial3 RX ×1, Serial4 TX ×1, AK4619VN PDN ×1, DESPEE boot control ×2)
Available for GPIO: **26**

### GPIO Requirements

| Function | Pins needed | Notes |
|----------|------------|-------|
| Encoder Nav-X (Back) | **3** | A, B, push switch |
| Encoder Nav-Y (Enter) | **3** | A, B, push switch |
| Encoder Edit (Menu) | **3** | A, B, push switch |
| Button A | **1** | Momentary, active-low with pull-up |
| Button B | **1** | Momentary, active-low with pull-up |
| Button C | **1** | Momentary, active-low with pull-up |
| 12× pads (4×3 matrix) | **7** | 4 row + 3 column scan lines |
| Pad LEDs (shift register) | **3** | SER, SRCLK, RCLK (e.g. 2× 74HC595) |
| SD card (SPI0) | **4** | CS + SCK (13) + MOSI (11) + MISO (12) — exclusive, no longer shared with display |
| Status LEDs | **1** | Power only (MIDI LED moved to 74HC595) |
| **Total** | **~23** | |

**Remaining spare:** ~3 pins. DESPEE display module uses dedicated Serial1 UART + boot control pins (counted under peripherals, not GPIO). MIDI activity LED moved to a spare 74HC595 shift register output (output 13 of 16).

------

## GPIO Pin Assignments

| Teensy Pin | Assignment | Notes |
|-----------|-----------|-------|
| **0** | Serial1 RX (DESPEE UART) | DESPEE TX → Teensy (touch events, status) |
| **1** | Serial1 TX (DESPEE UART) | Teensy → DESPEE RX (widget commands) |
| **2** | AK4619VN PDN | Codec power-down control (active-low) |
| **3** | Pad matrix row 2 | 4×3 matrix scanning |
| **4** | Pad matrix row 3 | 4×3 matrix scanning |
| **5** | Pad matrix col 0 | 4×3 matrix scanning |
| **6** | SAI1 RX_DATA1 | ADC2 stereo input |
| **7** | SAI1 TX_DATA0 | DAC1 stereo output |
| **8** | SAI1 RX_DATA0 | ADC1 stereo input |
| **9** | Pad matrix col 1 | 4×3 matrix scanning |
| **10** | Pad matrix row 0 | 4×3 matrix scanning (relocated from pin 0) |
| **11** | SPI0 MOSI | SD card only |
| **12** | SPI0 MISO | SD card only |
| **13** | SPI0 SCK | SD card only |
| **14** | Pad matrix col 2 | 4×3 matrix scanning |
| **15** | Serial3 RX | MIDI IN |
| **16** | SD card CS | SPI0 chip select for SD |
| **17** | Serial4 TX | MIDI OUT |
| **18** | Wire SDA | I2C bus |
| **19** | Wire SCL | I2C bus |
| **20** | SAI1 LRCLK | I2S frame sync |
| **21** | SAI1 BCLK | I2S bit clock |
| **22** | DESPEE ESP32_EN | Active-high enable; pull-up on module |
| **23** | SAI1 MCLK | I2S master clock (built-in LED unavailable) |
| **24** | Encoder Nav-X A | Quadrature channel A |
| **25** | Encoder Nav-X B | Quadrature channel B |
| **26** | Encoder Nav-X SW | Push switch |
| **27** | Encoder Nav-Y A | Quadrature channel A |
| **28** | Encoder Nav-Y B | Quadrature channel B |
| **29** | Encoder Nav-Y SW | Push switch |
| **30** | Encoder Edit A | Quadrature channel A |
| **31** | Encoder Edit B | Quadrature channel B |
| **32** | SAI1 TX_DATA1 | DAC2 stereo output |
| **33** | Encoder Edit SW | Push switch |
| **34** | Button A | Active-low, internal pull-up |
| **35** | Button B | Active-low, internal pull-up |
| **36** | Button C | Active-low, internal pull-up |
| **37** | DESPEE ESP32_GPIO0 | Boot mode: LOW = UART bootloader |
| **38** | Pad LED SER | 74HC595 serial data |
| **39** | Pad LED SRCLK | 74HC595 shift clock |
| **40** | Pad LED RCLK | 74HC595 latch clock |
| **41** | Pad matrix row 1 | 4×3 matrix scanning (relocated from pin 1; MIDI LED on 74HC595) |

------

## Conflict Warnings

1. **Serial2 (pins 7/8) is BLOCKED** — these pins are SAI1 audio data lines
2. **Pin 23 = LED** — built-in LED unavailable during audio playback
3. **Pin 6 = SAI1 RX_DATA1** — not available for general GPIO
4. **Pin 32 = SAI1 TX_DATA1** — not available for general GPIO
5. **Serial1 (pins 0/1) consumed by DESPEE** — not available for pad matrix (rows 0/1 relocated to pins 10/41)
