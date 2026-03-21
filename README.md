# SYNTEE

**Open-source DSP module for synths, effects, samplers, and more.**

Built around the Teensy 4.1, SynTee is a compact desktop DSP module designed for electronic musicians, synth enthusiasts, and DIY audio builders. A base firmware layer exposes the hardware — codec, MIDI, network audio, controls — while a swappable functional layer on top defines the device's role: synthesizer, effects processor, sampler, or anything else that fits the I/O. The primary function is a MIDI-controlled synthesizer, but the platform is open to any DSP workload.


![](hardware/syntee-layout.png)

## Features

### Hardware

- **Open** — Modular: main board (4-layer PCB), rear jack board, and external [DESPEE](https://github.com/openaudiotools/despee) display module; easy to source parts; easy to modify and recombine
- **Robust** — External [PHONEE](https://github.com/openaudiotools/phonee) headphone module (TPA6132A2 amp, 1/4" TRS jack, volume pot) via 4-pin JST-PH; panel-accessible SD card for presets, samples, and firmware updates; quality audio via AK4619VN codec (2× stereo in + 2× stereo out, 48 kHz / 24-bit, 3.5mm jacks)
- **Essential** — One main role, repurposable by firmware swap; 12 LED-backlit pads (2×6 grid), 3 rotary encoders (Nav-X, Nav-Y, Edit), 3 buttons (A, B, C) for focused control

### Software

- **Open** — Swappable functional layer defines device role (synth, effects, sampler, or anything that fits the I/O)
- **Robust** — UI offloaded to [DESPEE](https://github.com/openaudiotools/despee) (ESP32-S3 + LVGL, 4.3" 800×480 touchscreen), DSP stays on Teensy
- **Essential** — Single active function at a time; PJRC Audio Library block-based DSP (128 samples @ 48 kHz)

### Connectivity

- **Open** — AES67 network audio TX + RX (stereo out to DAW / mixer; stereo in from network); mDNS/DNS-SD discovery (`synth-XXXX.local`); network MIDI 2.0; no proprietary protocols
- **Robust** — USB MIDI host; MIDI IN/OUT via 3.5mm TRS Type A; works standalone without a computer; USB-C 5V power (no PD negotiation needed)
- **Essential** — Ethernet carries both audio and MIDI; standardized connectors throughout; USB Audio capable (reserved for future USB Audio Class 2 support)

## Architecture

SynTee uses a single AK4619VN codec (all 4 channels) on an I2S bus, driven by the Teensy 4.1's Cortex-M7 at 600 MHz. The base firmware exposes audio I/O, MIDI, network, and controls; a functional layer runs on top to define device behavior. In synth mode (the default), MIDI input drives a software synthesizer engine built on the PJRC Audio Library. Audio is routed to dual stereo DAC outputs and simultaneously packetized as AES67 RTP for network streaming. A stereo AES67 RX stream can also be received from the network and mixed into the audio router. Two stereo inputs allow external audio processing and effects returns. Power is supplied via a dedicated USB-C 5V input.

The diagram below shows the signal flow in synth mode:

```
                                               ┌──→ AK4619VN DAC1 ──→ OUT 1 L/R
MIDI IN ──┐                                    │
           ├──→ DSP Engine (Teensy 4.1)   ──→──┼──→ AK4619VN DAC2 ──→ OUT 2 L/R
Network ──┘         ▲                          │
                    │                          ├──→ PHONEE module ──→ Headphones
AK4619VN ADC1 ◄── IN 1 L/R                    │
AK4619VN ADC2 ◄── IN 2 L/R                    ├──→ AES67 TX ──→ Ethernet
                                               │
AES67 RX ◄── Ethernet ──────────────────────── ┘
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
│   ├── network-connectivity.md ← AES67 streaming, mDNS discovery, PTP sync
│   └── journal/           ← development journal
├── hardware/              ← schematics, PCB, mechanical
│   ├── lib/               ← shared KiCad footprint library
│   │   └── syntee-footprints.pretty/
│   └── pcbs/
│       └── main/              ← main board (4-layer)
│           ├── README.md      ← board concept, dimensions, key ICs
│           ├── connections.md ← connector pinouts
│           ├── architecture.md ← circuit details
│           ├── CLAUDE.md      ← agent guidance
│           └── designs/       ← KiCad files + gerbers
├── firmware/              ← Teensy 4.1 firmware (PlatformIO)
│   ├── platformio.ini
│   ├── src/main.cpp
│   └── lib/               ← project-local libraries
└── LICENSE
```

## Status

[SYNTEE issue](https://github.com/openaudiotools/about/issues/1) · [Project board](https://github.com/orgs/openaudiotools/projects/2)

## Discussions

Questions, ideas, and feedback are welcome in the [SYNTEE discussion category](https://github.com/openaudiotools/about/discussions/categories/syntee) on the Open Audio Tools repo.

## Contributing

See the [Contributing guide](https://github.com/openaudiotools/about/blob/main/docs/contributing.md) in the Open Audio Tools repo.

## License

SYNTEE is fully open source. Hardware is licensed under CERN-OHL-P v2 (permissive), firmware under MIT, and documentation under CC BY 4.0. See [LICENSE](LICENSE) for details.

**Author:** Juliusz Fedyk — [openaudiotools.com](https://openaudiotools.com)
