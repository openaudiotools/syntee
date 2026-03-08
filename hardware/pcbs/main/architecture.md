# Main Board — Architecture

---

## AK4619VN Codec Configuration

- **Address:** 0x10 (CAD pin tied low)
- **Interface:** I2S (not TDM — only stereo needed)
- **Clock:** Teensy generates MCLK (12.288 MHz), BCLK (3.072 MHz), LRCLK (48 kHz)
- **Codec is I2S slave** — all clocks from Teensy SAI1
- **Channels used:** ADC1 L/R (stereo input), DAC1 L/R (stereo output)
- **Channels unused:** ADC2 L/R, DAC2 L/R (available for future expansion)
- **PDN (Power Down):** Connected to GPIO pin 2. Active-low — hold low during power-up, release after supplies and MCLK are stable.

### Reset Sequencing

The AK4619VN requires a specific power-up sequence:

1. Apply power (5V → ADP7118 → 3.3V_A). PDN held low by GPIO pin 2.
2. Wait ≥10 ms for power rails to stabilize.
3. Start MCLK output (12.288 MHz) from Teensy SAI1.
4. Wait ≥1 ms for MCLK to stabilize.
5. Release PDN (set GPIO pin 2 HIGH).
6. Wait ≥10 ms for codec internal reset to complete.
7. Write I2C register configuration.

### I2C Register Configuration

Key registers to set at init (after reset sequence completes):
- Power Management: Enable ADC1 + DAC1, disable ADC2 + DAC2
- Audio Format: I2S, 24-bit, slave mode
- Clock: External MCLK, 256fs
- Volume: Set DAC output level via digital volume register

### Analog Stages

- **Op-amp:** OPA1678 (single-supply rail-to-rail output, 10 MHz GBW, 4.5 nV/√Hz). Biased at AVDD/2 (1.65V) virtual ground for single-supply 3.3V_A operation.
- **Input buffer:** Unity-gain OPA1678 buffer before ADC, biased at AVDD/2
- **Anti-alias filter (input):** 1st-order RC LPF at ADC input. R = 1 kΩ, C = 3.3 nF → f_c ≈ 48 kHz. Sufficient given AK4619VN internal oversampling and digital decimation filter.
- **Output buffer:** OPA1678 buffer after DAC output, biased at AVDD/2
- **Reconstruction filter (output):** 2nd-order Sallen-Key LPF at ~40 kHz after DAC output. R1 = R2 = 1 kΩ, C1 = 3.9 nF, C2 = 1.8 nF → f_c ≈ 40 kHz, Q ≈ 0.7 (Butterworth). Uses second OPA1678 section as Sallen-Key buffer.
- **Maximum expected input level:** +4 dBu (~1.23 Vrms, ~1.74 Vpk) — line level, consumer/prosumer

---

## MIDI IN Circuit (6N138 Optocoupler)

Standard MIDI opto-isolated input:

```
TRS Tip ──→ 220Ω ──→ 6N138 LED anode
                      6N138 LED cathode ←── 1N4148 (protection) ←── TRS Ring
                      6N138 collector ──→ 3.3V via 270Ω pull-up
                      6N138 emitter ──→ Serial3 RX (pin 15)
                      6N138 VCC ──→ 3.3V
                      6N138 GND ──→ GND
                      Pin 7 (VB) ──→ GND via 4.7kΩ (speed-up)
```

31,250 baud. No galvanic connection between MIDI sender and SynTee circuit ground.

## MIDI OUT Circuit (3.3V, per MMA CA-033)

```
3.3V ──→ 10Ω ──→ TRS Tip (source, pin 2)
Serial4 TX (pin 17) ──→ 33Ω ──→ TRS Ring (sink, pin 3)
```

3.3V MIDI current-loop output per MMA CA-033 for 3.3V sources. The 10Ω resistor is on the current-source side (Tip), and the 33Ω resistor is on the TX-driven sink side (Ring). When TX is LOW (active), current flows: 3.3V → 10Ω → Tip → receiver optocoupler → Ring → 33Ω → TX pin (LOW). With a typical receiver optocoupler forward voltage of ~1.2V, drive current ≈ (3.3V − 1.2V) / (10Ω + 33Ω) ≈ **49 mA** — well above the 5 mA minimum required by MIDI spec.

**Note:** TRS Type A mapping: Tip = pin 2 (source/5V), Ring = pin 3 (sink/signal).

---

## Power Entry

```
USB connector ──→ Polyfuse (1A hold) ──→ 5V rail
                                          ├──→ Teensy VIN (onboard 3.3V reg for digital)
                                          ├──→ ADP7118 LDO → 3.3V_A (codec analog supply)
                                          └──→ AP2553 USB power switch → USB Host VBUS
```

- **No isolated power domains** — single board, single ground plane
- **ADP7118:** Ultra-low-noise LDO for codec analog power (3.3V_A)
- **AP2553:** USB power switch IC on host VBUS line — provides overcurrent limiting (~500 mA), thermal shutdown, and enable control via GPIO. Protects against shorted or misbehaving USB devices.
- **Decoupling:** 0.1 µF ceramic at every IC, 10 µF bulk near LDO output

### USB Host Power Protection

The USB Host VBUS output is protected by an AP2553 (or equivalent) power switch IC:
- **Current limit:** ~500 mA (matches USB 2.0 downstream port spec)
- **Thermal shutdown:** Automatic cutoff on sustained overcurrent
- **Enable:** Directly controlled via firmware (always enabled after boot, can be disabled for power management)
- **Fault flag:** Active-low output to GPIO for overcurrent detection (optional — can be left unconnected for V1)

---

## Audio Input Protection

Each audio input has series resistance before the ESD clamp diodes to limit clamp current from overvoltage signals:

```
1/4" jack ──→ 1 kΩ series resistor ──→ BAT54 clamp diodes (to 3.3V_A and GND) ──→ OPA1678 input buffer
```

- **Series resistor:** 1 kΩ limits clamp current to ~4.5 mA at +20 dBu (~7.75 Vpk) worst case. Also forms part of the anti-alias RC filter with the 3.3 nF cap at the ADC input.
- **BAT54 clamp rails:** Anode to GND, cathode to 3.3V_A. Clamps input to –0.3V to 3.6V range.
- **Maximum safe input:** +20 dBu (~7.75 Vpk) without exceeding BAT54 current ratings.

---

## Control Interface (TBD)

Placeholder — control interface will be defined as the project progresses:

- **Encoders:** 2× rotary encoders with push switch (panel-mount, via 5-pin headers)
- **Display:** Small OLED (128×64, I2C) or small LCD (SPI) — TBD. Pin allocation (SPI vs I2C) should be finalized before layout begins.
- **Status LEDs:** Power indicator, MIDI activity

---

## Test Points

The following test points should be placed on the board for prototype debugging:

| Test Point | Signal | Location Notes |
|-----------|--------|----------------|
| TP1 | 5V rail | Near power entry, after polyfuse |
| TP2 | 3.3V_A | Near ADP7118 output |
| TP3 | I2S MCLK (12.288 MHz) | Near Teensy pin 23 |
| TP4 | I2C SDA | Near codec I2C connection |
| TP5 | I2C SCL | Near codec I2C connection |
| TP6 | MIDI IN data | At 6N138 emitter output |
| TP7 | AK4619VN PDN | At GPIO pin 2 |

Use SMD test point pads (1.5 mm round) — accessible with scope probe or test clip.

---

[Back to README](README.md)
