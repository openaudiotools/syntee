# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Important: do NOT cd into the current doc when calling commands.

## Project Overview

SYNTEE is an open-source DSP module built around the Teensy 4.1. A base firmware layer exposes hardware (codec, MIDI, network, controls); a swappable functional layer defines the device role (synth, effects, sampler, etc.). The primary function is a MIDI-controlled synthesizer. This is a **hardware-first project** currently in the design/documentation phase — no firmware code exists yet. The `firmware/` directory contains a minimal PlatformIO stub.

## Repository Structure

- `docs/` — System-level design specifications
- `hardware/pcbs/main/` — Single board documentation + design files
  - `README.md` — Board concept (lightweight, any agent can skim)
  - `connections.md` — Connector pinouts and interface contracts
  - `architecture.md` — Deep implementation detail (circuits, registers, etc.)
  - `CLAUDE.md` — Agent guidance for board work
  - `designs/` — KiCad files, scripts, gerbers
- `hardware/lib/` — Shared KiCad footprint library
- `firmware/` — PlatformIO project (Teensy 4.1 + PJRC Audio Library)

## Common Commands (Windows)

| Tool | Path |
|------|------|
| kicad-cli | `D:\programs\KiCad\9.0\bin\kicad-cli.exe` |
| pcbnew Python | `D:\programs\KiCad\9.0\bin\python.exe` |

```bash
# DRC check (fill zones first via MCP or pcbnew Python — kicad-cli doesn't auto-fill)
"D:\programs\KiCad\9.0\bin\kicad-cli.exe" pcb drc --format json --severity-all --units mm -o drc-report.json module.kicad_pcb

# Gerber export (4-layer board)
"D:\programs\KiCad\9.0\bin\kicad-cli.exe" pcb export gerbers \
  -l "F.Cu,In1.Cu,In2.Cu,B.Cu,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts" \
  --subtract-soldermask -o ./gerbers/ module.kicad_pcb

# Drill file export
"D:\programs\KiCad\9.0\bin\kicad-cli.exe" pcb export drill -o ./gerbers/ module.kicad_pcb

# PDF export for review
"D:\programs\KiCad\9.0\bin\kicad-cli.exe" pcb export pdf \
  -l "F.Cu,In1.Cu,In2.Cu,B.Cu,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts" \
  --drill-shape-opt 2 -o ./module.pdf module.kicad_pcb
```

## MCP Servers

Two KiCad MCP servers are configured:
- **kicadmixelpixx** — generation, placement, routing, DRC, Gerber export (SWIG backend, requires KiCad 9.0+). Gerber export via MCP may produce empty files — use kicad-cli instead.
- **kicadseed** — schematic analysis, netlist reading, pin tracing, design review.

Prefer MCP tools over raw Python/CLI when possible.

## Documentation Loading Guide

**PCB work on the board:**
1. Read `docs/hardware.md` — key ICs, power budget, target specs
2. Read `hardware/pcbs/main/README.md`, `connections.md`, `architecture.md`
3. Read `docs/pcb-design-rules.md` if doing layout/routing
4. Read `docs/pin-mapping.md` for Teensy pin assignments

**Firmware work:**
- Read `docs/firmware.md` — synth engine, audio pipeline, network stack
- Read `docs/hardware.md` for key ICs and target specs
- Read `docs/pin-mapping.md` for Teensy pin assignments
- For display work: see [DESPEE](https://github.com/openaudiotools/despee) protocol and integration docs

**Network / connectivity:**
- Read `docs/network-connectivity.md` — AES67 streaming, mDNS, PTP

## Architecture at a Glance

**Audio path:** MIDI → Teensy 4.1 DSP engine → AK4619VN DAC (2× stereo out) → analog out + AES67 TX over Ethernet. 2× stereo ADC inputs for external audio / effects return. AES67 RX from network feeds into the audio router. The DSP engine's behavior is defined by the active functional layer (synth, effects, sampler, etc.).

**I2S interface:** Single AK4619VN codec on I2S (not TDM — 4 channels: 2× stereo in + 2× stereo out). Teensy is I2S master, codec is slave.

**I2C control:** AK4619VN at address 0x10 (CAD pin low), on Wire (pins 18/19). No mux needed — single codec.

**Key hardware ICs:** AK4619VN (codec), MAX97220 (headphone amp), 6N138 (MIDI optocoupler), RJ45 MagJack (Ethernet with integrated magnetics). **External module:** [DESPEE](https://github.com/openaudiotools/despee) display module (ESP32-S3 + 4.3" 800×480 LCD via 6-pin JST-PH UART header).

**Connectivity:** Stereo AES67 network audio TX + RX via Ethernet (DP83825I PHY on Teensy 4.1, RJ45 MagJack on-board). USB MIDI host. MIDI IN/OUT via 3.5mm TRS Type A.

**Power:** Dedicated USB-C 5V input (no PD negotiation). Simple — no STUSB4500, no isolated DC-DC, no galvanic isolation needed on a single board. Separate PC USB-C for firmware/MIDI.

## Key Documentation Cross-References

| Topic | Primary doc | Also update |
|-------|------------|-------------|
| Pin assignments | `docs/pin-mapping.md` | `hardware/pcbs/main/connections.md`, `architecture.md` |
| Component changes | `docs/hardware.md` | Board `architecture.md` |
| Connector pinouts | `hardware/pcbs/main/connections.md` | `docs/pin-mapping.md` |
| Display protocol | [DESPEE protocol](https://github.com/openaudiotools/despee) | `docs/firmware.md`, `hardware/pcbs/main/architecture.md` |

## When Editing Documentation

- **Always check cross-references.** Pin counts, connector specs, and power figures appear in multiple files. Use grep to find all occurrences before considering a change complete.

## Firmware (When It Exists)

- **Platform:** Arduino/Teensyduino with PJRC Audio Library, built via PlatformIO
- **Audio:** Block-based DSP (128 samples @ 48 kHz = 2.67 ms blocks), runs in timer interrupt
- **Real-time constraint:** Audio callback preempts everything. No blocking operations, no long SPI transactions, no malloc in the audio path.
- **Key libraries:** PJRC Audio, QNEthernet, USBHost_t36, Bounce, Encoder, esp-serial-flasher (DESPEE firmware update), ArduinoJson (ui.json parsing)

## Licensing

Triple-licensed: MIT (firmware), CERN-OHL-P v2 (hardware), CC BY 4.0 (docs).

@docs/journal/GUIDE.md
