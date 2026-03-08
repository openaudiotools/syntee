# Main Board — Connections

All connectors on the single SynTee board. Since there's only one PCB, all connections are panel-mount or on-board headers.

---

## Audio Output (1/4" TS Jacks, ×2)

| Jack | Signal | Notes |
|------|--------|-------|
| OUT L | AK4619VN DAC1 L | Left channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |
| OUT R | AK4619VN DAC1 R | Right channel, via 2nd-order Sallen-Key reconstruction filter (~40 kHz) + OPA1678 output buffer |

**Connector:** 1/4" TS panel-mount jacks. BAT54 ESD diodes on each jack.

**Note:** 1/4" jacks are physically large. If enclosure size becomes a constraint, consider 3.5mm TRS jacks instead — common on modern compact synth modules.

---

## Audio Input (1/4" TS Jacks, ×2)

| Jack | Signal | Notes |
|------|--------|-------|
| IN L | AK4619VN ADC1 L | Left channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |
| IN R | AK4619VN ADC1 R | Right channel, via 1 kΩ series resistor → BAT54 clamp → OPA1678 input buffer |

**Connector:** 1/4" TS panel-mount jacks. 1 kΩ series resistor before BAT54 ESD/overvoltage clamp diodes (to 3.3V_A and GND) on each jack. Maximum safe input: +20 dBu.

**Note:** 1/4" jacks are physically large. If enclosure size becomes a constraint, consider 3.5mm TRS jacks instead — common on modern compact synth modules.

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

## USB Power Input

USB-B or USB-C connector for 5V power. Through polyfuse to 5V rail.

---

## Encoder Headers (×2, pin headers)

| Pin | Signal |
|-----|--------|
| 1 | Encoder A |
| 2 | Encoder B |
| 3 | Push switch |
| 4 | GND |
| 5 | 3.3V (pull-up supply) |

5-pin 2.54mm headers for panel-mount encoders. Teensy pin assignments TBD.

---

## Display Header (TBD)

Pin header for small OLED or LCD display. Interface (I2C or SPI) depends on display choice. Pinout will be defined when display is selected.

---

[Back to README](README.md)
