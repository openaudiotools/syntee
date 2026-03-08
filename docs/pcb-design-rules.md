# SYNTEE: PCB Design Rules

*← Back to [README](../README.md) | See also: [Hardware](hardware.md)*

------

SynTee follows the same PCB design rules as [MixTee](../../mixtee/docs/pcb-design-rules.md), adapted for a single 4-layer board. Key parameters are summarized below.

## Manufacturing Target

Primary: **JLCPCB** or **PCBWay** standard capabilities. All design rules chosen to be within standard process limits (no advanced/special process charges).

------

## Trace Widths

| Net Class | Trace Width | Use |
|-----------|------------|-----|
| Default (digital signal) | 0.25 mm (10 mil) | General digital signals, I2C, GPIO |
| Audio_Analog | 0.30 mm (12 mil) | Codec analog I/O, op-amp paths |
| Power | 0.5–1.0 mm (20–40 mil) | 5V, 3.3V, GND power distribution |

------

## Clearances

| Parameter | Value | Notes |
|-----------|-------|-------|
| Trace-to-trace (minimum) | 0.15 mm (6 mil) | JLCPCB standard minimum |
| Trace-to-pad | 0.15 mm | |
| Pad-to-pad | 0.15 mm | |
| Analog-to-digital signal spacing | ≥0.5 mm preferred | Keep analog traces away from digital switching signals |
| Courtyard-to-courtyard | 0.25 mm | Component placement clearance |

------

## Via Sizes

| Via Type | Drill Diameter | Annular Ring | Use |
|----------|---------------|-------------|-----|
| Standard signal | 0.3 mm (12 mil) | 0.15 mm | General signal vias |
| Power | 0.4 mm (16 mil) | 0.2 mm | 5V, 3.3V, GND power vias |
| Thermal via array | 0.3 mm | 0.15 mm | Under QFN exposed pad (AK4619VN); 4–9 vias |

------

## Copper Weight

| Layer Position | Weight | Notes |
|---------------|--------|-------|
| Outer layers (Top, Bottom) | 1 oz (35 µm) | Standard |
| Inner layers (GND, PWR) | 0.5 oz (17.5 µm) | Standard 4-layer inner core |

**4-layer stackup:**

| Layer | Function | Copper |
|-------|----------|--------|
| L1 (Top) | Signal + components | 1 oz |
| L2 | GND plane (continuous) | 0.5 oz |
| L3 | Power plane (5V / 3.3V) | 0.5 oz |
| L4 (Bottom) | Signal + components | 1 oz |

------

## Grounding Strategy

Single continuous GND plane on L2. No galvanic isolation needed — single board with shared ground. Star topology for high-current returns converging near power entry.

### Mixed-Signal Grounding Detail

The AK4619VN is the critical mixed-signal boundary. Follow the AK4619VN datasheet reference layout:

- **Codec analog section:** Keep analog bypass capacitors (AVDD decoupling) close to codec analog supply pins. Route analog ground returns through short traces back to these bypass caps — do not let analog return currents flow through the digital ground path.
- **Single-point merge:** Analog and digital ground currents merge at one point near the codec, underneath or immediately adjacent to the QFN pad. Do not split the ground plane — use a single continuous plane, but be mindful of current return paths.
- **Placement guidance:** Place OPA1678 op-amps, analog filter passives, and audio jack connections in the analog zone adjacent to the codec. Keep digital switching signals (SPI, USB, Ethernet) routed away from this area.
- **Decoupling return paths:** Each bypass capacitor's ground pad should have a via to L2 (GND plane) directly at the pad. No long traces to ground vias.

### Differential Pair Routing

| Pair | Impedance | Notes |
|------|-----------|-------|
| Ethernet TX+/TX- | 100Ω differential | Short, direct route from Teensy bottom pads to MagJack. No external coupling caps. |
| Ethernet RX+/RX- | 100Ω differential | Short, direct route from MagJack to Teensy bottom pads. |
| USB Host D+/D- | 90Ω differential | From Teensy bottom pads to USB-A connector. Minimize trace length. |

------

## QFN Pad Design

For AK4619VN QFN-32:

- **Exposed pad:** Solder paste coverage ~60–70% (windowed stencil)
- **Thermal vias:** 4–9 vias (0.3 mm drill) in exposed pad, connected to ground plane
- **Via-in-pad:** Fill and cap if budget allows; otherwise via tenting on bottom side

------

## Silkscreen

- **Minimum line width:** 0.15 mm (6 mil)
- **Minimum text height:** 1.0 mm
- **Reference designators:** On all components
- **Board markings:** Board name, revision, date, CERN-OHL-P v2 notice, openmusictools.com
- **Polarity indicators:** On all polarized components
- **Pin 1 markers:** On all ICs

------

## Board Outline

- **Edge clearance:** ≥0.3 mm from board edge to nearest copper
- **Mounting holes:** M3 (3.2 mm drill) with 6 mm pad, connected to GND
- **Corner radius:** 1.0 mm minimum (prevents stress cracks)

------

## Design Rule Check (DRC) Settings

Apply these as KiCad net class rules:

```
Net class: Default
  Track width: 0.25mm
  Clearance: 0.2mm
  Via size: 0.6mm (0.3mm drill)

Net class: Power
  Track width: 0.5mm
  Clearance: 0.2mm
  Via size: 0.8mm (0.4mm drill)

Net class: Audio_Analog
  Track width: 0.3mm
  Clearance: 0.25mm
  Via size: 0.6mm (0.3mm drill)
```
