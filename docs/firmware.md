# SYNTEE: Firmware

*← Back to [README](../README.md) | See also: [Hardware](hardware.md) · [Pin Mapping](pin-mapping.md) · [Network Connectivity](network-connectivity.md)*

------

## Firmware Framework

- **Platform:** Arduino/Teensyduino (PJRC Audio Library), built via PlatformIO
- **Audio Processing:** Block-based (128 samples @ 48 kHz = 2.67 ms latency)
- **Core function:** MIDI-controlled synthesizer engine

------

## Key Libraries

- **Audio:** PJRC Audio Library (AudioSynthWaveform, AudioFilterStateVariable, AudioEffectEnvelope, AudioMixer4, AudioOutputI2S)
- **Encoders:** PJRC Encoder Library (quadrature decoding)
- **Buttons:** PJRC Bounce Library (debouncing)
- **Network:** QNEthernet (TCP/IP stack, mDNS, UDP)
- **MIDI:** USBHost_t36 (USB MIDI host), Serial MIDI (TRS)
- **Display:** TBD — depends on display choice (OLED via I2C, or small LCD via SPI)

------

## Audio Pipeline Overview

```
MIDI IN ──────────────────────────┐
  (USB Host / TRS / Network)      │
                                  ▼
                          ┌───────────────┐
                          │  MIDI Parser  │
                          │  (note on/off,│
                          │   CC, pitch)  │
                          └───────┬───────┘
                                  │
                                  ▼
                          ┌───────────────┐
                          │  Synth Engine │
                          │  (oscillators,│
                          │   filters,    │
                          │   envelopes,  │
                          │   effects)    │
                          └───────┬───────┘
                                  │
                    ┌─────────────┼─────────────┐
                    ▼                             ▼
            ┌───────────────┐             ┌───────────────┐
            │  I2S TX       │             │  RTP Encode   │
            │  (AK4619VN    │             │  (AES67 TX    │
            │   DAC out)    │             │   to network) │
            └───────────────┘             └───────────────┘
```

### Synth Engine (Planned)

The synthesizer architecture will be built from PJRC Audio Library objects:

- **Oscillators:** AudioSynthWaveform / AudioSynthWaveformModulated (saw, square, sine, triangle)
- **Filters:** AudioFilterStateVariable (LP/HP/BP with resonance)
- **Envelopes:** AudioEffectEnvelope (ADSR for amplitude and filter cutoff)
- **Mixer:** AudioMixer4 (voice mixing, effects send/return)
- **Effects:** AudioEffectDelay, AudioEffectChorus (basic built-in effects)

Polyphony target: **8 voices** of a typical subtractive synth patch (osc + filter + envelope) is realistic given ~25% CPU usage. 16 voices with effects may exceed CPU budget — benchmark on real hardware before committing. Start with 8 and optimize.

------

## Network Stack

- **QNEthernet** for TCP/IP, UDP, mDNS
- **AES67 TX:** RTP packetizer encodes stereo audio into L24/48000/2 packets (1 ms packet time, 48 samples/ch)
- **PTP slave:** IEEE 1588v2 software timestamping via Teensy GPT timer (~50–200 µs accuracy in practice)
- **mDNS/DNS-SD:** Announces `synth-XXXX.local` and `_jfa-audio._udp` / `_jfa-midi2._udp` services

See [network-connectivity.md](network-connectivity.md) for full details.

------

## Real-Time Constraints

The PJRC Audio Library runs on a timer interrupt and preempts all other code:

- **Audio callback:** 128 samples @ 48 kHz = 2.67 ms block time. Synth engine runs here.
- **No blocking in audio path:** No malloc, no long I2C transactions, no Serial prints.
- **Main loop:** Handles MIDI parsing (event-driven), encoder/button polling, display updates, network (QNEthernet event loop, RTP/PTP).

### CPU Budget Estimate

| Subsystem | Estimated CPU |
|-----------|---------------|
| I2S receive (stereo) | ~1% |
| I2S transmit (stereo) | ~1% |
| Synth engine (8 voices) | ~15–25% |
| RTP audio encode | ~1% |
| Network stack (QNEthernet + mDNS + PTP) | ~3–5% |
| MIDI parsing | <1% |
| UI updates | <1% |
| **Total** | **~25–35%** |

**~65% CPU headroom** remains — room for more voices, effects, or future features.

------

## Codec Initialization

The AK4619VN requires a specific power-up/reset sequence before audio can begin. See [architecture.md](../hardware/pcbs/main/architecture.md) for the full sequence. In firmware:

```cpp
// In setup():
pinMode(2, OUTPUT);
digitalWrite(2, LOW);        // Hold PDN low
delay(10);                    // Wait for power rails
// ... start I2S / MCLK ...
delay(1);                     // Wait for MCLK stable
digitalWrite(2, HIGH);        // Release PDN
delay(10);                    // Wait for codec reset
// ... write I2C registers ...
```

## Hardware Watchdog

The Teensy 4.1 has a hardware watchdog timer (WDOG1) — **use it**. A hung synth module on stage is unacceptable.

- Enable watchdog in `setup()` with a 1–2 second timeout
- Pet the watchdog in the main loop (not in the audio interrupt)
- If the main loop hangs (e.g., network stack deadlock), the watchdog resets the MCU
- Use the PJRC `Watchdog_t4` library or direct register access

## Priority Model

```
Audio Callback (interrupt, ~2.67 ms):
  +-- I2S RX (codec ADC → memory, if used)
  +-- Synth engine (oscillators, filters, envelopes)
  +-- I2S TX (memory → codec DAC)
  +-- [RTP encode] — if audio streaming enabled

Main Loop (low priority):
  +-- Watchdog pet
  +-- Ethernet RX (DMA-driven)
  +-- UDP/TCP socket operations (QNEthernet event loop)
  +-- RTP/PTP message processing
  +-- mDNS daemon (polled, ~100 ms interval)
  +-- MIDI parsing (USB host ISR + Serial RX)
  +-- Display updates
  +-- Encoder/button polling
```
