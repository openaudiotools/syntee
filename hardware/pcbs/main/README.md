# Main Board

**Dimensions:** TBD | **Layers:** 4 | **Orientation:** Horizontal | **Instances:** 1

Single board for the entire SynTee synthesizer. Houses the Teensy 4.1 (on socket headers), AK4619VN codec, MIDI interface, Ethernet MagJack, power entry, and all user controls. All connectors are panel-mount (audio jacks, MIDI TRS, RJ45, USB).

## Key ICs

- Teensy 4.1 on socket headers (+ optional PSRAM)
- AK4619VN — 4-in/4-out codec (stereo in + stereo out used), PDN controlled by GPIO pin 2
- OPA1678 — Dual audio op-amp (×2) for input buffers and output Sallen-Key filter (single-supply 3.3V_A)
- 6N138 — MIDI IN optocoupler
- RJ45 MagJack — Ethernet with integrated magnetics (direct connection, no external coupling caps)
- ADP7118 — LDO (5V → 3.3V_A for codec analog supply)
- AP2553 — USB host VBUS power switch (overcurrent protection)

## Functional Zones

1. **Digital zone:** Teensy 4.1, USB host, Ethernet, MIDI circuits
2. **Analog zone:** AK4619VN codec, op-amp stages, audio jacks
3. **Power zone:** USB power entry, polyfuse, LDO
4. **Control zone:** Encoder headers, display header, status LEDs

## Board Zoning

Physical placement map driven by noise isolation. The north edge is the top panel (user-facing) with audio jacks and MIDI connectors. The noisiest digital connectors (Ethernet 25 MHz, USB Host 12 MHz) sit at the south edge (rear), farthest from the sensitive analog circuitry. The Teensy bridges the two domains in the center.

```
    NORTH (analog / quiet — top panel)
┌──────────────────────────────────────────────────────┐
│ [USB-B/C]  [IN L] [IN R]  [OUT L] [OUT R] [MIDI] [MIDI]│
│  Power      Audio Inputs    Audio Outputs   IN     OUT  │
│  + polyfuse   1/4" TS        1/4" TS      3.5mm  3.5mm │
│                                                      │
│        [OPA1678 ×2 + filter passives]                │
│        [6N138 optocoupler]  [BAT54 ESD diodes]       │
│                                                      │
│   ┌─────────────┐  ┌──────────────────┐  [Encoder]  │
│   │  ADP7118    │  │  AK4619VN codec  │  [Headers]  │
│   │  LDO        │  │  AVDD ← west     │  [×2]      │
│   │  3.3V_A     │  │  DVDD ← south    │             │
│   └─────────────┘  │  analog ← north  │  [Display]  │
│                     └──────────────────┘  [Header]   │
│                                                      │
│  ┌──────────────────────────────────────┐            │
│  │          Teensy 4.1                  │            │
│  │       (on socket headers)            │            │
│  │  I2S / I2C ←  north end             │            │
│  │  ETH pads ←  south end              │            │
│  │  USB host pads ←  south end         │            │
│  └──────────────────────────────────────┘            │
│                                                      │
│         ·····AP2553·····                              │
│  [RJ45 MagJack]              [USB-A Host]            │
│   Ethernet                    USB MIDI Host           │
└──────────────────────────────────────────────────────┘
    SOUTH (digital / noisy — rear)
```

### Key Placement Rules

- **Ethernet and audio on opposite edges.** RJ45 MagJack at south, audio jacks at north — maximizes physical separation between 25 MHz Ethernet clocks and sensitive analog signals.
- **Codec orientation.** AK4619VN analog pins (VCOM, AOUTL/R, AINL/R) face north toward audio jacks; digital pins (SDATA, BCLK, LRCK) face south toward Teensy I2S pads.
- **MCLK guard spacing.** MCLK trace from Teensy to codec stays within the analog/codec zone, routed ≥ 1 mm from Ethernet differential pairs and USB D+/D− traces.
- **Op-amps near codec.** OPA1678s and associated Sallen-Key filter passives within 15 mm of AK4619VN analog pins to minimize trace length in the sensitive signal path.
- **Analog power locality.** ADP7118 LDO placed adjacent to codec, with 3.3V_A output decoupling capacitors at the codec AVDD pins (< 5 mm).
- **No digital routing through analog zone.** No digital signal traces cross the analog zone (codec + op-amps + audio jacks) on the outer copper layers (F.Cu / B.Cu). Digital signals use inner layers or route around the zone.

## See Also

- [`connections.md`](connections.md) — all connector pinouts
- [`architecture.md`](architecture.md) — codec config, MIDI circuit, power entry, control interface

## Status

Not started. Specifications being defined.
