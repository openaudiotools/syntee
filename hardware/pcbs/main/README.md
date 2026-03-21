# Main Board

**Dimensions:** 140 × 200 mm | **Layers:** 4 | **Orientation:** Horizontal | **Instances:** 1

Main board for SynTee. Houses the Teensy 4.1 (on socket headers), AK4619VN codec, MIDI interface, Ethernet MagJack, power entry, 6-pin JST-PH header for DESPEE display module, 4-pin JST-PH header for [PHONEE](https://github.com/openaudiotools/phonee) headphone module, and all user controls (3 encoders, 3 buttons, 12 LED-backlit pads). All connectors are on the single main board — no separate jack board. The [DESPEE](https://github.com/openaudiotools/despee) display module and [PHONEE](https://github.com/openaudiotools/phonee) headphone module mount externally.

## Key ICs

- Teensy 4.1 on socket headers (+ optional PSRAM)
- AK4619VN — 4-in/4-out codec (all channels used: 2× stereo in + 2× stereo out), PDN controlled by GPIO pin 2
- 6-pin JST-PH header for [DESPEE](https://github.com/openaudiotools/despee) display module (Serial1 UART + boot control)
- 4-pin JST-PH header for [PHONEE](https://github.com/openaudiotools/phonee) headphone module (TPA6132A2 amp, 1/4" TRS jack, volume pot)
- OPA1678 — Dual audio op-amp (×4) for input buffers and output Sallen-Key filters (single-supply 3.3V_A)
- 6N138 — MIDI IN optocoupler
- Amphenol ACJM-MHD — Dual stacked 1/4" mono (TS) jacks (×4 units = 8 jacks), horizontal snap-fit PCB mount
- RJ45 MagJack — Ethernet with integrated magnetics (direct connection, no external coupling caps)
- ADP7118 — LDO (5V → 3.3V_A for codec analog supply)
- AP2553 — USB host VBUS power switch (overcurrent protection)

## Functional Zones

1. **Digital zone:** Teensy 4.1, USB host, Ethernet, MIDI circuits
2. **Analog zone:** AK4619VN codec, OPA1678 op-amp stages (×4), audio jacks (4× Amphenol ACJM-MHD dual 1/4" TS)
3. **Power zone:** USB-C power entry, polyfuse, LDO
4. **Control zone:** 3× encoder headers, 3× button headers, 12× pad matrix + LED shift registers
5. **Display zone:** 6-pin JST-PH header for DESPEE display module (Serial1 UART + boot control)
6. **PHONEE zone:** 4-pin JST-PH header for external [PHONEE](https://github.com/openaudiotools/phonee) headphone module

## Board Zoning

Physical placement map driven by noise isolation and the 4-edge panel layout. Connectors are distributed across all edges matching the front-panel layout design.

```
                    NORTH (top edge — audio + MIDI)
     ┌──────────────────────────────────────────────────────┐
     │  [OUT2 L/R]  [OUT1 L/R]  [MIDI OUT] [MIDI IN]  [IN2 L/R]  [IN1 L/R]  │
     │  ACJM-MHD ×2 (dual 1/4")        3.5mm TRS      ACJM-MHD ×2 (dual 1/4") │
     │                                                                        │
     │      [OPA1678 ×4 + filter passives]   [BAT54 ESD]   [6N138]           │
W    │                                                                        │  E
E    │  ┌─────────┐  ┌──────────────────┐                                    │  A
S    │  │ ADP7118 │  │  AK4619VN codec  │  ┌──────────────────┐             │  S
T    │  │ LDO     │  │  AVDD ← west     │  │  DESPEE header  │  [PHONEE]  │  T
     │  │ 3.3V_A  │  │  DVDD ← south    │  │  (6-pin JST-PH,│  4-pin     │
[PC] │  └─────────┘  │  analog ← north  │  │   Serial1 UART)│  JST-PH   │ (right
USB-C│                └──────────────────┘  └──────────────────┘            │  edge)
     │                                                                        │
[PWR]│  ┌──────────────────────────────────────┐  [Nav-X][Nav-Y][Edit]       │
USB-C│  │          Teensy 4.1                  │   Encoders ×3               │
     │  │       (on socket headers)            │                              │
[SD  │  │  I2S / I2C ←  north end             │  [A] [B] [C]                │
card]│  │  ETH pads ←  south end              │   Buttons ×3                │
     │  │  USB host pads ←  south end         │                              │
     │  └──────────────────────────────────────┘  ┌────────────────────────┐ │
     │                                             │  12× Pads (2×6 grid)  │ │
     │         ·····AP2553·····                    │  + 74HC595 LED drivers │ │
     │  [RJ45 MagJack]              [USB-A Host]  └────────────────────────┘ │
     │   Ethernet                    MIDI Host                               │
     └──────────────────────────────────────────────────────────────────────┘
                    SOUTH (rear edge — Ethernet + USB host)
```

### Key Placement Rules

- **Ethernet and audio on opposite edges.** RJ45 MagJack at south (rear), audio jacks at north (top) — maximizes physical separation between 25 MHz Ethernet clocks and sensitive analog signals.
- **Codec orientation.** AK4619VN analog pins (VCOM, AOUTL/R, AINL/R) face north toward audio jacks; digital pins (SDATA, BCLK, LRCK) face south toward Teensy I2S pads.
- **MCLK guard spacing.** MCLK trace from Teensy to codec stays within the analog/codec zone, routed ≥ 1 mm from Ethernet differential pairs and USB D+/D− traces.
- **Op-amps near codec.** OPA1678s (×4) and associated Sallen-Key filter passives within 15 mm of AK4619VN analog pins to minimize trace length in the sensitive signal path.
- **Analog power locality.** ADP7118 LDO placed adjacent to codec, with 3.3V_A output decoupling capacitors at the codec AVDD pins (< 5 mm).
- **No digital routing through analog zone.** No digital signal traces cross the analog zone (codec + op-amps + audio jacks) on the outer copper layers (F.Cu / B.Cu). Digital signals use inner layers or route around the zone.
- **DESPEE header near Serial1 pins.** Place the 6-pin JST-PH DESPEE header near Teensy Serial1 pins (0/1) and boot control pins (22/37). Short UART traces minimize noise coupling.
- **PHONEE header near codec.** 4-pin JST-PH header placed near DAC1 output (east side). Short traces from DAC1 reconstruction filter output to the header minimize noise pickup on the analog audio lines going to the external PHONEE module.
- **Control cluster.** Encoders, buttons, and pad matrix grouped in the center-south area below the display, away from the analog zone.

## See Also

- [`connections.md`](connections.md) — all connector pinouts
- [`architecture.md`](architecture.md) — codec config, MIDI circuit, power entry, control interface

## Status

Not started. Specifications being defined.
