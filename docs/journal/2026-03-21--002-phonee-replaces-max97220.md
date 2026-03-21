# 002 — PHONEE replaces MAX97220 onboard headphone amp

**Date:** 2026-03-21
**Scope:** hardware docs, board architecture, connections, power budget

## Summary

Removed the onboard MAX97220 headphone amplifier circuit from SynTee's main board and replaced it with a 4-pin JST-PH header connecting to the external [PHONEE](https://github.com/openaudiotools/phonee) headphone module. PHONEE is a reusable headphone output board (TPA6132A2 amp, 1/4" TRS jack, PCB-mount volume pot) already used by MIXTEE. This aligns SynTee with the modular approach used across OpenAudioTools projects.

## What changed

| File | Change |
|------|--------|
| `README.md` | MAX97220 reference → PHONEE module; updated signal flow diagram |
| `CLAUDE.md` | Updated key ICs and external modules list |
| `docs/hardware.md` | Power budget: 10 mA (MAX97220) → 30 mA (PHONEE); updated key ICs table; adjusted total estimate to ~650–750 mA |
| `docs/firmware.md` | Audio pipeline diagram: MAX97220 → PHONEE |
| `hardware/pcbs/main/README.md` | Board description, key ICs, functional zones, board zoning diagram, placement rules — all updated |
| `hardware/pcbs/main/architecture.md` | Replaced MAX97220 circuit section with PHONEE module section (JST-PH connector, integration notes, grounding) |
| `hardware/pcbs/main/connections.md` | Replaced 3.5mm TRS headphone jack section with 4-pin JST-PH PHONEE connector section |

Also added SynTee to PHONEE's "Used In" list in the PHONEE repo.

### Removed from main board

- MAX97220 stereo headphone amplifier IC
- 3.5mm TRS headphone jack (panel-mount)
- Analog volume potentiometer (panel-mount, 10k log)
- "Headphone zone" as a dedicated board area

### Added to main board

- 4-pin JST-PH right-angle header (S4B-PH-K-S) on east edge, near DAC1 output
- Pin 1 = Audio L, Pin 2 = Audio R, Pin 3 = 5V, Pin 4 = GND

### Design decisions

- **No galvanic isolation:** SynTee is a single-board, single-ground-plane design. PHONEE's integration guide recommends isolation, but it's not practical here — noise managed by zone separation instead.
- **5V power to PHONEE:** Recommended over 3.3V for full headphone output headroom.
- **Headphone detect not routed:** PHONEE has a detect switch, but it's left unconnected in V1 — no firmware use case yet.
- **No pin mapping changes:** MAX97220 wasn't consuming any Teensy GPIO pins, so the pin budget is unchanged.

## Context

The PHONEE repo was created as a standalone, reusable headphone output module — same pattern as DESPEE for display. Centralizing the headphone circuit in its own repo avoids duplicating the design across MIXTEE and SynTee, and lets each project just provide a JST-PH header and cable. The 1/4" TRS jack on PHONEE is an upgrade from the 3.5mm jack that was planned with the MAX97220.
