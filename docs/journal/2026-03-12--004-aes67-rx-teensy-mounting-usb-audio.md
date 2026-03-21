# 004 — Add AES67 RX, document Teensy mounting, future-proof USB Audio

**Date:** 2026-03-12
**Scope:** Documentation updates across 8 files — no firmware or schematic changes

## What changed

### AES67 RX

SynTee previously only documented a TX (transmit) stream. Added a full AES67 RX (receive) path:

- RTP depacketizer receives one stereo stream (L24/48000/2) via IGMP multicast subscription
- ~4 ms jitter buffer feeds decoded PCM into the Audio Router alongside local ADC inputs
- Use cases: DAW returns, backing tracks, click/metronome from network
- SAP listener discovers remote TX streams; RX SDP documents expected format
- Bandwidth updated to ~5 Mbps total (TX + RX), still well within 100 Mbps
- CPU budget: +1% for RTP decode + jitter buffer

### Teensy mounting

Documented the socketed header approach for the Teensy 4.1:

- Edge pins (0–41): male headers on Teensy into female sockets on main board
- Bottom pads (USB Host, Ethernet): same header/socket approach
- PC USB-C: routes directly from Teensy's onboard connector (not through headers)
- Impedance analysis: 100 Mbps Ethernet and USB Full Speed tolerate header discontinuity; PC USB-C needs 90Ω differential for High Speed

### USB Audio future-proofing

- PC USB-C noted as USB 2.0 High Speed capable, reserved for future USB Audio Class 2
- 90Ω differential trace requirement documented for signal integrity
- Added to README feature list

## MicroMod evaluation outcome

Evaluated MicroMod as an alternative to socketed Teensy 4.1 — **not viable**. Staying with Teensy 4.1 on socketed headers.

## Files modified

| File | Changes |
|------|---------|
| `README.md` | AES67 TX+RX in features, ASCII diagram, architecture text, USB Audio bullet |
| `CLAUDE.md` | Audio path and connectivity descriptions |
| `docs/hardware.md` | DAW connectivity TX+RX, PC USB-C USB Audio note |
| `docs/firmware.md` | Audio pipeline diagram (RTP Decode block), AES67 RX description, CPU budget |
| `docs/network-connectivity.md` | RX stream service, RX SDP, IGMP, jitter buffer, SAP listener, bandwidth, firmware impact |
| `docs/pin-mapping.md` | Teensy Mounting section with impedance notes |
| `hardware/pcbs/main/architecture.md` | Teensy Mounting section with impedance considerations |
| `hardware/pcbs/main/connections.md` | Ethernet/USB Host header+socket notes, USB PC USB Audio note |
