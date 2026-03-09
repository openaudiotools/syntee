# Main Board — Connections

All connectors on the single SynTee board. Since there's only one PCB, all connections are panel-mount or on-board headers.

---

## Audio Output (3.5mm TS Jacks, ×4)

All audio outputs on the **top edge** of the panel.

| Jack | Signal | Notes |
|------|--------|-------|
| OUT 1 L | AK4619VN DAC1 L | Left channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |
| OUT 1 R | AK4619VN DAC1 R | Right channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |
| OUT 2 L | AK4619VN DAC2 L | Left channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |
| OUT 2 R | AK4619VN DAC2 R | Right channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |

**Connector:** 3.5mm TS panel-mount jacks. BAT54 ESD diodes on each jack.

---

## Audio Input (3.5mm TS Jacks, ×4)

All audio inputs on the **top edge** of the panel.

| Jack | Signal | Notes |
|------|--------|-------|
| IN 1 L | AK4619VN ADC1 L | Left channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |
| IN 1 R | AK4619VN ADC1 R | Right channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |
| IN 2 L | AK4619VN ADC2 L | Left channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |
| IN 2 R | AK4619VN ADC2 R | Right channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |

**Connector:** 3.5mm TS panel-mount jacks. 1 kΩ series resistor before BAT54 ESD/overvoltage clamp diodes (to 3.3V_A and GND) on each jack. Maximum safe input: +20 dBu.

---

## MIDI IN (3.5mm TRS Type A)

| Pin | Signal |
|-----|--------|
| Tip | MIDI current loop + (through 6N138 optocoupler) |
| Ring | MIDI current loop - |
| Sleeve | Shield / GND (not connected to circuit GND) |

Standard MIDI IN circuit with 6N138 optocoupler → Serial3 RX (pin 15).

---

## MIDI OUT (3.5mm TRS Type A)

| Pin | Signal |
|-----|--------|
| Tip | MIDI current source (3.3V via 10Ω) |
| Ring | MIDI current sink (Serial4 TX via 33Ω) |
| Sleeve | Shield / GND |

3.3V MIDI OUT per MMA CA-033 from Serial4 TX (pin 17). Tip sources current, Ring sinks via TX pin.

---

## Ethernet (RJ45 MagJack)

RJ45 with integrated magnetics. Teensy Ethernet bottom pads connect directly to the MagJack through short, impedance-controlled differential traces (100Ω differential). No external coupling capacitors — the Teensy 4.1 PHY has on-chip AC coupling, and the MagJack has integrated magnetics with built-in isolation. Follow PJRC Teensy 4.1 Ethernet reference design.

---

## USB Host (USB-A Socket)

Single USB-A socket for MIDI controllers. Connected to Teensy USB Host bottom pads (D+, D-, GND). USB 2.0 differential pairs require 90Ω impedance-controlled routing. Place Teensy near the board edge closest to the USB-A connector to minimize trace length.

**VBUS protection:** 5V supplied through AP2553 USB power switch IC (overcurrent limiting ~500 mA, thermal shutdown). Protects against shorted or misbehaving USB devices.

---

## Headphone Output (3.5mm TRS Jack)

Located on the **right edge** of the panel.

| Pin | Signal | Notes |
|-----|--------|-------|
| Tip | Left audio | MAX97220 output L |
| Ring | Right audio | MAX97220 output R |
| Sleeve | GND | Signal ground |

**Connector:** 3.5mm TRS panel-mount jack. Fed from AK4619VN DAC1 output via MAX97220 stereo headphone amplifier. An analog potentiometer (panel-mount, right edge below headphone jack) controls headphone volume — no GPIO pin needed for volume control.

---

## USB — PC (Teensy Native USB-C)

Located on the **left edge** of the panel.

Teensy 4.1 native USB-C connector. Used for:
- Firmware updates (USB DFU)
- USB MIDI device mode (Teensy appears as a MIDI device to the host PC)
- Serial debug console

---

## USB — PWR (Power-Only USB-C)

Located on the **left edge** of the panel, below PC USB.

Dedicated USB-C connector for 5V power input only (no data lines connected). Through polyfuse to 5V rail. Provides power to the entire board.

---

## SD Card Socket

Located on the **left edge** of the panel, below the USB connectors.

Panel-accessible micro SD card socket. Connected via SPI0 (exclusive access — no longer shared with display). CS on pin 16. Supports FAT32 and exFAT for preset storage, sample loading, and configuration files.

---

## Encoder Headers (×3, pin headers)

| Pin | Signal |
|-----|--------|
| 1 | Encoder A |
| 2 | Encoder B |
| 3 | Push switch |
| 4 | GND |
| 5 | 3.3V (pull-up supply) |

5-pin 2.54mm headers for panel-mount encoders.

| Encoder | Role | Teensy Pins (A/B/SW) |
|---------|------|---------------------|
| **Nav-X** | Horizontal navigation, push = Back | 24 / 25 / 26 |
| **Nav-Y** | Vertical navigation, push = Enter | 27 / 28 / 29 |
| **Edit** | Value editing, push = Menu | 30 / 31 / 33 |

---

## Buttons (×3, momentary)

| Button | Teensy Pin | Notes |
|--------|-----------|-------|
| **A** | 34 | Active-low, internal pull-up |
| **B** | 35 | Active-low, internal pull-up |
| **C** | 36 | Active-low, internal pull-up |

Panel-mount momentary push buttons. No external pull-up resistors needed — Teensy internal pull-ups used.

---

## Pads (×12, tactile switches with LED backlighting)

12 tactile switch pads arranged in a 2×6 grid on the front panel. Each pad has an individual LED for backlighting.

### Pad Matrix (4×3 scan)

| | Col 0 (pin 5) | Col 1 (pin 9) | Col 2 (pin 14) |
|---|---|---|---|
| **Row 0 (pin 10)** | Pad 1 | Pad 2 | Pad 3 |
| **Row 1 (pin 41)** | Pad 4 | Pad 5 | Pad 6 |
| **Row 2 (pin 3)** | Pad 7 | Pad 8 | Pad 9 |
| **Row 3 (pin 4)** | Pad 10 | Pad 11 | Pad 12 |

**Note:** The physical 2×6 grid maps to a 4×3 electrical matrix for efficient GPIO usage.

### LED Backlighting

12 LEDs driven via 2× 74HC595 shift registers (12 of 16 outputs used). Controlled by 3 GPIO pins: SER (pin 38), SRCLK (pin 39), RCLK (pin 40).

---

## DESPEE Display Module (6-pin JST-PH)

| Pin | Signal | Teensy Pin |
|-----|--------|-----------|
| 1 | 5V | — |
| 2 | GND | — |
| 3 | UART TX (Teensy → DESPEE) | 1 |
| 4 | UART RX (DESPEE → Teensy) | 0 |
| 5 | ESP32_EN | 22 |
| 6 | ESP32_GPIO0 | 37 |

External [DESPEE](https://github.com/openaudiotools/despee) display module — custom ESP32-S3 PCB with 4.3" 800×480 capacitive touch LCD running LVGL. Connected via Serial1 UART at 921600 baud. Boot control pins (EN/GPIO0) used for firmware updates via esp-serial-flasher.

See [DESPEE integration guide](https://github.com/openaudiotools/despee/blob/main/docs/integration-guide.md).

---

[Back to README](README.md)
