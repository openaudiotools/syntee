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
- **Display:** Offloaded to [DESPEE](https://github.com/openaudiotools/despee) display module (ESP32-S3 running LVGL); Teensy streams binary widget commands over Serial1 UART at 921600 baud; see DESPEE protocol
- **SD Card:** SdFat or SD library (SPI0 — exclusive, no longer shared)
- **DESPEE update:** esp-serial-flasher (Espressif) for reflashing DESPEE firmware from SD card
- **UI layout:** ArduinoJson — streaming JSON parser for ui.json (UI layout from SD card)
- **Pad LEDs:** 74HC595 shift register driver (SER/SRCLK/RCLK GPIO)

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
                    ┌─────────────┤
                    ▼             ▼
            ┌───────────────┐  ┌───────────────┐
            │  I2S RX       │  │  Synth Engine │
            │  (AK4619VN    │  │  (oscillators,│
            │   ADC1+ADC2   │  │   filters,    │
            │   stereo in)  │  │   envelopes,  │
            └───────┬───────┘  │   effects)    │
                    │          └───────┬───────┘
                    │                  │
                    ▼    mix / route   ▼
                    ┌─────────────────────┐
                    │     Audio Router    │
                    │  (mix, FX, routing) │
                    └──────────┬──────────┘
                               │
              ┌────────────────┼────────────────┐
              ▼                ▼                 ▼
      ┌───────────────┐ ┌───────────────┐ ┌───────────────┐
      │  I2S TX       │ │  Headphone    │ │  RTP Encode   │
      │  (AK4619VN    │ │  (MAX97220    │ │  (AES67 TX    │
      │   DAC1+DAC2   │ │   from DAC)   │ │   to network) │
      │   stereo out) │ │               │ │               │
      └───────────────┘ └───────────────┘ └───────────────┘
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
| I2S receive (2× stereo) | ~2% |
| I2S transmit (2× stereo) | ~2% |
| Synth engine (8 voices) | ~15–25% |
| RTP audio encode | ~1% |
| Network stack (QNEthernet + mDNS + PTP) | ~3–5% |
| MIDI parsing | <1% |
| UI updates (DESPEE UART) | <1% |
| Pad scanning + LED update | <1% |
| SD card access (non-audio) | <1% |
| **Total** | **~30–40%** |

**~60% CPU headroom** remains — room for more voices, effects, or future features.

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
  +-- Display updates (DESPEE UART — Serial1)
  +-- Encoder/button polling (3× encoders, 3× buttons)
  +-- Pad matrix scanning + LED shift register update
  +-- SD card file operations (preset load/save)
```

------

## Display Module (DESPEE)

Display rendering is offloaded to the [DESPEE](https://github.com/openaudiotools/despee) module — a custom ESP32-S3 PCB with a 4.3" 800×480 capacitive touch LCD running LVGL. The Teensy communicates with DESPEE over Serial1 UART at 921600 baud:

- **Protocol:** Binary widget commands (COBS-encoded, CRC16) — see DESPEE protocol docs
- **UI layout:** Loaded from `ui.json` on SD card at boot, streamed to DESPEE as widget create/update commands
- **Touch input:** DESPEE forwards touch coordinates back to Teensy over the same UART
- **Boot control:** ESP32_EN (pin 22) and ESP32_GPIO0 (pin 37) for reset and firmware flashing
- **Non-blocking:** UART writes in main loop, never in audio interrupt. Serial1 TX buffer handles bursts.

Display updates run in the main loop, not the audio interrupt. The DESPEE module boots independently and renders autonomously — Teensy only sends widget state changes.

------

## Pad Scanning and LED Control

### Pad Matrix (4×3)

The 12 tactile switch pads are wired as a 4-row × 3-column scan matrix:

- **Scan rate:** ~1 kHz (1 ms per full scan) in main loop
- **Debounce:** Software debounce per pad (Bounce library or custom, ~5 ms)
- **Velocity sensing:** Not supported (digital on/off only — tactile switches)

### LED Backlighting

Each pad has an individual LED driven via 2× 74HC595 shift registers (12 outputs used of 16):

- **Update rate:** Shift out new LED state after each pad scan (~1 kHz)
- **PWM dimming:** Not per-LED (shift register is on/off). Global brightness can be controlled via a series resistor or by varying the shift register update duty cycle.
- **GPIO pins:** SER (pin 38), SRCLK (pin 39), RCLK (pin 40)

------

## UI Navigation Model

### Encoders

| Encoder | Role | Turn action | Push action |
|---------|------|-------------|-------------|
| **Nav-X** | Horizontal navigation | Scroll left/right, change tabs | Back / cancel |
| **Nav-Y** | Vertical navigation | Scroll up/down, change parameter | Enter / confirm |
| **Edit** | Value editing | Adjust selected value | Open menu / context action |

### Buttons

| Button | Role |
|--------|------|
| **A** | Context-dependent action (e.g., play, arm, toggle) |
| **B** | Context-dependent action (e.g., stop, mute, shift) |
| **C** | Context-dependent action (e.g., record, solo, alt) |

### Pads

12 pads arranged in a 2×6 grid. Pad functions depend on the active mode:
- **Trigger mode:** Each pad triggers a note or sample
- **Sequencer mode:** Pads represent steps in a sequence
- **Preset mode:** Pads select presets or banks

------

## SD Card Access

The panel-accessible SD card socket connects via SPI0 (exclusive access — no longer shared with display):

- **CS pin:** Pin 16
- **Library:** SdFat (supports FAT32, exFAT)
- **Use cases:** Preset storage (JSON or binary), sample loading, firmware update files, user configuration
- **Access pattern:** File I/O happens only in the main loop, never in the audio interrupt. Large reads (sample loading) should be chunked to avoid blocking the main loop for too long.

**Note:** The Teensy 4.1 also has a built-in SD card socket on the bottom of the board (SDIO interface). The panel-accessible SD card on SPI0 is a separate, user-facing socket for convenient access without opening the enclosure.
