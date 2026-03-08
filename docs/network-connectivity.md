# SYNTEE: Network Connectivity

*← Back to [README](../README.md) | See also: [Hardware](hardware.md) · [Firmware](firmware.md)*

------

SynTee participates in the OpenAudioTools network ecosystem defined in the [shared network connectivity plan](../../mixtee/docs/network-connectivity.md). This document describes SynTee's specific role.

## 1. Network Stack

- **Hardware:** Teensy 4.1 with native 100 M Ethernet (DP83825I RMII PHY)
- **Software:** QNEthernet library (UDP/TCP/IP stack)
- **Network layer:** IPv4 only, single LAN, no routing
- **IP addressing:** DHCP with IPv4 link-local fallback (`169.254.x.x`)

------

## 2. Hostname and Identity

- **Hostname:** `synth-XXXX.local` (where `XXXX` = last 4 hex digits of MAC address)
- **Role:** `synth`
- **mDNS:** Announces hostname at boot, answers queries

------

## 3. Published Services

### Audio TX — `_jfa-audio._udp`

SynTee publishes one stereo audio TX stream:

| Service Instance | TXT Fields |
|-----------------|------------|
| `Synth Out 1-2._jfa-audio._udp.local` | `role=synth`, `dir=tx`, `ch=2`, `sr=48000`, `fmt=pcm24`, `pkt=1`, `stream=main` |

**RTP payload:** L24 (24-bit linear PCM), 48 kHz, stereo, 1 ms packet time (48 samples/ch).
**Packet size:** 48 × 2 × 3 = 288 bytes payload — fits easily in one Ethernet frame.

### MIDI 2.0 — `_jfa-midi2._udp`

| Service Instance | TXT Fields |
|-----------------|------------|
| `SynTee._jfa-midi2._udp.local` | `dir=inout`, `ump=2.0`, `ep=synth`, `ch=16` |

SynTee accepts MIDI input (note on/off, CC, pitch bend, program change) and optionally sends MIDI feedback (parameter echo, clock).

------

## 4. Time Sync (PTP)

- **Role:** PTP **slave** (IEEE 1588v2)
- **PTP domain:** 0 (AES67 default)
- Syncs to grandmaster on the network (typically the mixer)
- Software PTP via Teensy GPT timer (~50–200 µs accuracy in practice — software timestamping on a non-deterministic Arduino main loop limits precision; 10 µs is unlikely without hardware timestamping)
- RTP timestamps derived from PTP clock

------

## 5. AES67 Compliance

SynTee implements full AES67 for DAW interoperability:

### SDP (Session Description Protocol)

```
v=0
o=- <session-id> <version> IN IP4 <device-ip>
s=SynTee main
c=IN IP4 <unicast-ip>
t=0 0
m=audio <port> RTP/AVP 96
a=rtpmap:96 L24/48000/2
a=ptime:1
a=ts-refclk:ptp=IEEE1588-2008:<ptp-grandmaster-id>
a=mediaclk:direct=0
```

### SAP (Session Announcement Protocol)

- Multicasts SDP to **239.255.255.255:9875** every 30 seconds
- Virtual soundcards on DAW host auto-discover the stream

### Bandwidth

- TX: 2 ch × 48 kHz × 24 bit × 1.1 overhead ≈ **2.5 Mbps**
- Well within 100 Mbps Ethernet capacity

------

## 6. Firmware Impact

| Component | CPU impact |
|-----------|-----------|
| RTP packetizer (stereo) | <1% |
| SAP/SDP announcer | <0.5% |
| PTP slave | 2–5% |
| mDNS/DNS-SD | <0.5% |
| QNEthernet event loop | 2–3% |
| **Total network** | **~5–9%** |
