# Main Board — Architecture

---

## Teensy Mounting

The Teensy 4.1 is socketed onto the main board — not soldered directly:

- **Edge pins (0–41):** Male pin headers soldered to the Teensy, inserted into female socket headers on the main board
- **Bottom pads (USB Host, Ethernet):** Short male pin headers soldered to the Teensy underside pads, inserted into matching female sockets on the main board
- **PC USB-C:** Routes directly from the Teensy's onboard USB-C connector to the panel cutout — not through headers

This allows the Teensy to be removed for debugging, replacement, or reuse without desoldering.

### Impedance Considerations

The header/socket interface introduces a small impedance discontinuity on the bottom-pad signals:

- **Ethernet (100 Mbps):** Tolerant of the discontinuity — short traces to MagJack, 100Ω differential target. The DP83825I PHY's internal equalization handles minor reflections.
- **USB Host (Full Speed, 12 Mbps):** Tolerant — MIDI controllers use Full Speed USB, well within the error margin of socketed connections.
- **PC USB-C (High Speed, 480 Mbps):** Not routed through headers — goes directly from the Teensy's onboard connector. Maintain **90Ω differential** traces on the main board between the Teensy USB-C and the panel cutout to support future USB Audio Class 2.

---

## AK4619VN Codec Configuration

- **Address:** 0x10 (CAD pin tied low)
- **Interface:** I2S (not TDM — only stereo needed)
- **Clock:** Teensy generates MCLK (12.288 MHz), BCLK (3.072 MHz), LRCLK (48 kHz)
- **Codec is I2S slave** — all clocks from Teensy SAI1
- **Channels used:** ADC1 L/R (stereo input 1), ADC2 L/R (stereo input 2), DAC1 L/R (stereo output 1), DAC2 L/R (stereo output 2) — all 4 channels active
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
- Power Management: Enable ADC1 + ADC2 + DAC1 + DAC2 (all channels active)
- Audio Format: I2S, 24-bit, slave mode
- Clock: External MCLK, 256fs
- Volume: Set DAC output level via digital volume register

### Analog Stages

Each stereo pair (IN 1, IN 2, OUT 1, OUT 2) has its own OPA1678 dual op-amp for a total of **4× OPA1678** (8 op-amp sections).

- **Op-amp:** OPA1678 (single-supply rail-to-rail output, 10 MHz GBW, 4.5 nV/√Hz). Biased at AVDD/2 (1.65V) virtual ground for single-supply 3.3V_A operation.
- **Input buffer (×4):** Unity-gain OPA1678 buffer before each ADC channel, biased at AVDD/2
- **Anti-alias filter (input):** 1st-order RC LPF at each ADC input. R = 1 kΩ, C = 3.3 nF → f_c ≈ 48 kHz. Sufficient given AK4619VN internal oversampling and digital decimation filter.
- **Output buffer (×4):** OPA1678 buffer after each DAC output, biased at AVDD/2
- **Reconstruction filter (output):** 2nd-order Sallen-Key LPF at ~40 kHz after each DAC output. R1 = R2 = 1 kΩ, C1 = 3.9 nF, C2 = 1.8 nF → f_c ≈ 40 kHz, Q ≈ 0.7 (Butterworth). Uses second OPA1678 section as Sallen-Key buffer.
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

Each audio input (4 jacks total, via 2× ACJM-MHD dual jack units) has series resistance before the ESD clamp diodes to limit clamp current from overvoltage signals:

```
1/4" TS jack ──→ 1 kΩ series resistor ──→ BAT54 clamp diodes (to 3.3V_A and GND) ──→ OPA1678 input buffer
```

- **Series resistor:** 1 kΩ limits clamp current to ~4.5 mA at +20 dBu (~7.75 Vpk) worst case. Also forms part of the anti-alias RC filter with the 3.3 nF cap at the ADC input.
- **BAT54 clamp rails:** Anode to GND, cathode to 3.3V_A. Clamps input to –0.3V to 3.6V range.
- **Maximum safe input:** +20 dBu (~7.75 Vpk) without exceeding BAT54 current ratings.

---

## Headphone Output (PHONEE Module)

```
AK4619VN DAC1 L/R ──→ Sallen-Key filter ──→ 4-pin JST-PH header ──→ PHONEE module (external)
```

- **Module:** [PHONEE](https://github.com/openaudiotools/phonee) — external headphone output board (TPA6132A2 amp, 1/4" TRS jack, PCB-mount volume pot)
- **Input:** Tapped from DAC1 output (post reconstruction filter, pre output jack) via 4-pin JST-PH cable
- **Power:** 5V from main board (PHONEE has 3.3–5V input range; 5V recommended for full headroom)
- **Connector:** 4-pin JST-PH right-angle header on main board east edge. Pin 1 = Audio L, Pin 2 = Audio R, Pin 3 = 5V, Pin 4 = GND
- **Detect:** PHONEE has a headphone detect switch — not routed to Teensy in V1 (leave detect pad unconnected)
- **Grounding:** Shared ground plane (no isolation). SynTee is a single-board design — digital noise is managed by zone separation, not galvanic isolation

See [PHONEE integration guide](https://github.com/openaudiotools/phonee/blob/main/docs/integration-guide.md) for cable and mounting details.

---

## Display Module (DESPEE)

The display is an external [DESPEE](https://github.com/openaudiotools/despee) module — a custom ESP32-S3 PCB with a 4.3" 800×480 capacitive touch LCD running LVGL. Connected via a 6-pin JST-PH header:

- **Interface:** Serial1 UART at 921600 baud (TX pin 1, RX pin 0)
- **Protocol:** Binary widget commands (COBS-encoded, CRC16) — see DESPEE protocol docs
- **Boot control:** ESP32_EN (pin 22, active-high with pull-up on module), ESP32_GPIO0 (pin 37, LOW = UART bootloader)
- **Power:** 5V from main board (DESPEE has onboard 3.3V regulation)

### Boot Sequence

DESPEE boots independently — no init sequence required from Teensy. After Teensy starts, it loads `ui.json` from SD card and streams widget create/update commands over UART. Touch coordinates are sent back from DESPEE.

### Firmware Update (via SD card)

Teensy uses esp-serial-flasher to reflash DESPEE firmware: pull ESP32_GPIO0 (pin 37) LOW, pulse ESP32_EN (pin 22) to reset into UART bootloader, stream firmware binary over Serial1.

---

## Pad Scanning Circuit (4×3 Matrix + LED Backlighting)

### Switch Matrix

12 tactile switches in a 4-row × 3-column matrix:

```
        Col 0 (pin 5)    Col 1 (pin 9)    Col 2 (pin 14)
             │                 │                 │
Row 0 (pin 10)──┤── [SW1] ──────┤── [SW2] ──────┤── [SW3]
Row 1 (pin 41)──┤── [SW4] ──────┤── [SW5] ──────┤── [SW6]
Row 2 (pin 3)──┤── [SW7] ──────┤── [SW8] ──────┤── [SW9]
Row 3 (pin 4)──┤── [SW10] ─────┤── [SW11] ─────┤── [SW12]
```

- **Scanning:** Row pins configured as outputs (drive low one at a time), column pins as inputs with pull-ups. Read columns to detect pressed switches.
- **Diodes:** 1N4148 on each switch for N-key rollover (prevents ghost keypresses).
- **Scan rate:** ~1 kHz in main loop.

### LED Backlighting

12 LEDs (one per pad) driven by 2× 74HC595 8-bit shift registers daisy-chained:

```
Teensy pin 38 (SER)   ──→ 74HC595 #1 SER
Teensy pin 39 (SRCLK) ──→ 74HC595 #1/#2 SRCLK
Teensy pin 40 (RCLK)  ──→ 74HC595 #1/#2 RCLK
74HC595 #1 QH'        ──→ 74HC595 #2 SER
```

- **LED current:** ~5 mA per LED via series resistor (220Ω for 3.3V supply, or 150Ω for 5V supply)
- **Total LED current:** 12 × 5 mA = 60 mA max (if all LEDs on), typically ~25 mA average

---

## SD Card Interface

Panel-accessible micro SD card socket on the **left edge** of the panel.

- **Interface:** SPI0 (exclusive access — no longer shared with display)
  - CS: pin 16 (dedicated)
  - SCK/MOSI/MISO: pins 13/11/12
- **Power:** 3.3V supply

**Note:** This is separate from the Teensy 4.1's built-in SDIO card socket on the bottom of the board. The panel socket provides user-accessible storage without opening the enclosure.

---

## Control Interface

### Encoders (×3)

Rotary encoders with push switch, panel-mount via 5-pin headers:

| Encoder | A pin | B pin | SW pin | Role |
|---------|-------|-------|--------|------|
| Nav-X | 24 | 25 | 26 | Horizontal nav, push = Back |
| Nav-Y | 27 | 28 | 29 | Vertical nav, push = Enter |
| Edit | 30 | 31 | 33 | Value edit, push = Menu |

Hardware debouncing: 100 nF caps on A/B/SW lines to GND. Software: PJRC Encoder library (interrupt-driven quadrature decoding on A/B pins).

### Buttons (×3)

Momentary push buttons, panel-mount:

| Button | Pin | Notes |
|--------|-----|-------|
| A | 34 | Active-low, internal pull-up |
| B | 35 | Active-low, internal pull-up |
| C | 36 | Active-low, internal pull-up |

100 nF hardware debounce caps. Software: Bounce library.

### Status LEDs

- **MIDI activity:** Driven via 74HC595 shift register output 13 (pin 41 reassigned to pad matrix row 1)

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
