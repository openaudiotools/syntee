# SYNTEE

**Open-source virtual sound module / synthesizer with network audio and MIDI.**

Built around the Teensy 4.1, SynTee is a compact desktop synthesizer designed for electronic musicians, synth enthusiasts, and DIY audio builders. It produces stereo audio output via an AK4619VN codec, accepts MIDI input over USB, TRS, and Ethernet, and streams audio over AES67 — all on a single PCB.

## Features

- **Stereo audio output** via AK4619VN codec (48 kHz / 24-bit, I2S)
- **Stereo audio input** for external processing / effects return
- **MIDI input** via USB host + 3.5mm TRS Type A + network MIDI 2.0
- **AES67 network audio** TX (stereo out to DAW / mixer)
- **mDNS/DNS-SD discovery** — auto-announces as `synth-XXXX.local`
- **Basic controls** — encoders, buttons, small display (TBD)
- **Single-board design** — one 4-layer PCB, simple power via USB 5V

## Architecture

SynTee uses a single AK4619VN codec on an I2S bus, driven by the Teensy 4.1's Cortex-M7 at 600 MHz. MIDI input triggers a software synthesizer engine built on the PJRC Audio Library. Audio is routed to the codec DAC outputs and simultaneously packetized as AES67 RTP for network streaming. Power is supplied via USB 5V (no PD negotiation needed).

```
MIDI IN ──┐
           ├──→ Synth Engine (Teensy DSP) ──→ AK4619VN DAC ──→ Analog Out
Network ──┘                                  └──→ AES67 TX ──→ Ethernet
```

## Repository Structure

```
syntee/
├── README.md              ← you are here
├── LICENSE                ← MIT (firmware) + CERN-OHL-P v2 (hardware) + CC BY 4.0 (docs)
├── docs/                  ← system-level design specifications
│   ├── hardware.md        ← key ICs, power budget, target audio specs
│   ├── pin-mapping.md     ← Teensy 4.1 pin assignments for SynTee
│   ├── firmware.md        ← audio pipeline, synth engine, network stack
│   ├── pcb-design-rules.md ← trace widths, clearances, manufacturing rules
│   └── network-connectivity.md ← AES67 streaming, mDNS discovery, PTP sync
├── hardware/              ← schematics, PCB, mechanical
│   ├── lib/               ← shared KiCad footprint library
│   │   └── syntee-footprints.pretty/
│   └── pcbs/
│       └── main/              ← single board (4-layer)
│           ├── README.md      ← board concept, dimensions, key ICs
│           ├── connections.md ← connector pinouts
│           ├── architecture.md ← circuit details
│           ├── CLAUDE.md      ← agent guidance
│           └── designs/       ← KiCad files + gerbers
├── firmware/              ← Teensy 4.1 firmware (PlatformIO)
│   ├── platformio.ini
│   ├── src/main.cpp
│   └── lib/               ← project-local libraries
└── journal/               ← development journal
```

## Status

**Pre-prototype / Design phase.** Hardware and firmware specifications are being defined. This repository was just initialized.

## License

SYNTEE is fully open source. Hardware is licensed under CERN-OHL-P v2 (permissive), firmware under MIT, and documentation under CC BY 4.0. See [LICENSE](LICENSE) for details.

**Author:** Juliusz Fedyk — [openmusictools.com](https://openmusictools.com)
