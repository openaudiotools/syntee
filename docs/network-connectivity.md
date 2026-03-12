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

SynTee publishes one stereo audio TX stream and can subscribe to one stereo RX stream:

#### TX Stream

| Service Instance | TXT Fields |
|-----------------|------------|
| `Synth Out 1-2._jfa-audio._udp.local` | `role=synth`, `dir=tx`, `ch=2`, `sr=48000`, `fmt=pcm24`, `pkt=1`, `stream=main` |

**RTP payload:** L24 (24-bit linear PCM), 48 kHz, stereo, 1 ms packet time (48 samples/ch).
**Packet size:** 48 × 2 × 3 = 288 bytes payload — fits easily in one Ethernet frame.

#### RX Stream

| Service Instance | TXT Fields |
|-----------------|------------|
| `Synth In 1-2._jfa-audio._udp.local` | `role=synth`, `dir=rx`, `ch=2`, `sr=48000`, `fmt=pcm24`, `pkt=1`, `stream=return` |

SynTee subscribes to a remote AES67 multicast stream via IGMP join. The RX stream is depacketized and fed into the audio router as a stereo input (alongside the local ADC inputs). Useful for DAW returns, backing tracks, or click/metronome from the network.

**Jitter buffer:** ~4 ms (4 packets) to absorb network timing variation. Buffer depth is fixed at startup.

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

#### TX SDP (announced by SynTee)

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

#### RX SDP (subscribed by SynTee)

SynTee discovers remote TX streams via SAP announcements or manual SDP configuration. Expected format:

```
m=audio <port> RTP/AVP 96
a=rtpmap:96 L24/48000/2
a=ptime:1
```

The RX stream is subscribed by joining the multicast group from the remote SDP's `c=` line via IGMP.

### SAP (Session Announcement Protocol)

- **TX:** Multicasts SDP to **239.255.255.255:9875** every 30 seconds
- **RX:** Listens on **239.255.255.255:9875** for remote TX stream announcements; auto-discovers available streams
- Virtual soundcards on DAW host auto-discover the TX stream

### Bandwidth

- TX: 2 ch × 48 kHz × 24 bit × 1.1 overhead ≈ **2.5 Mbps**
- RX: 2 ch × 48 kHz × 24 bit × 1.1 overhead ≈ **2.5 Mbps**
- **Total:** ~5 Mbps — well within 100 Mbps Ethernet capacity

------

## 6. Firmware Impact

| Component | CPU impact |
|-----------|-----------|
| RTP packetizer TX (stereo) | <1% |
| RTP depacketizer RX (stereo) + jitter buffer | ~1% |
| SAP/SDP announcer + listener | <0.5% |
| PTP slave | 2–5% |
| mDNS/DNS-SD | <0.5% |
| QNEthernet event loop | 2–3% |
| **Total network** | **~6–10%** |
