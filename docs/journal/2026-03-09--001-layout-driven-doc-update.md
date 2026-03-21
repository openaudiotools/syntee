# Layout-Driven Documentation Update

**Date:** 2026-03-09
**Scope:** All design docs (7 files + CLAUDE.md)

---

## Summary

Updated all documentation to match the new front-panel/enclosure layout (`hardware/syntee-layout.png`). The layout significantly expands SynTee's feature set beyond the original minimal concept.

## Design Changes

| Area | Before | After |
|------|--------|-------|
| Dimensions | TBD | **140 × 200 mm** |
| Display | "Small OLED 128×64 or LCD — TBD" | **4.3" TFT, 480×272, RA8875 controller** |
| Audio jacks | 1/4" TS, 1 stereo pair in/out | **3.5mm TS, 2 stereo pairs in + 2 stereo pairs out** (all 4 AK4619VN channels) |
| Encoders | 2× rotary | **3×: Nav-X (Back), Nav-Y (Enter), Edit (Menu)** |
| Buttons | None | **3× momentary: A, B, C** |
| Pads | None | **12× tactile switch pads (2×6 grid)** with LED backlighting via 74HC595 shift registers |
| Headphones | None | **3.5mm TRS jack + analog volume pot, MAX97220 amp** |
| SD Card | Not mentioned | **Panel-accessible micro SD socket (SPI0, shared with display)** |
| USB | Single USB-B/C | **PC = Teensy native USB-C; PWR = dedicated power-only USB-C** |
| Panel layout | Top edge only | **4-edge: top=audio+MIDI, left=PC USB+PWR+SD, right=phones+vol, rear=MIDI HOST+ETH** |
| Op-amps | OPA1678 ×2 | **OPA1678 ×4** (each stereo pair gets its own buffer/filter set) |
| Power budget | ~325–375 mA | **~460–480 mA** |

## New Key ICs

- **RA8875** — TFT display controller (SPI0)
- **MAX97220** — Stereo headphone amplifier
- **74HC595 ×2** — Shift registers for pad LED backlighting

## GPIO Impact

Peripheral pins increased from 10 to 12 (added SAI1 TX_DATA1 pin 32, RX_DATA1 pin 6 for 4-channel I2S). GPIO budget is now tight: 30 available, ~30 consumed. SPI bus sharing (display + SD) and shift registers for pad LEDs were essential to fit.

## Files Modified

| File | Key changes |
|------|-------------|
| `docs/hardware.md` | All-channel codec, RA8875 + MAX97220 in key ICs, OPA1678 ×4, updated power budget, split USB, 140×200mm |
| `docs/pin-mapping.md` | 4-channel I2S (pins 6, 32), full GPIO assignments for all controls, tight budget analysis |
| `docs/firmware.md` | RA8875/SdFat/74HC595 libraries, 4-channel audio pipeline, display driver, pad scanning, UI nav model, SD card sections |
| `hardware/pcbs/main/README.md` | 140×200mm, 6 functional zones, 4-edge ASCII board zoning, new placement rules |
| `hardware/pcbs/main/connections.md` | 3.5mm jacks (×8), headphone, split USB, SD socket, 3 encoders, 3 buttons, 12 pads, RA8875 display |
| `hardware/pcbs/main/architecture.md` | All codec channels enabled, headphone circuit, RA8875 interface, pad matrix + LED drivers, SD card, full control interface |
| `README.md` | Updated features list and architecture diagram |
| `CLAUDE.md` | Updated architecture summary (4-channel, new ICs, split USB) |

## Verification

- No stale TBDs in `docs/` or `hardware/` markdown
- No 1/4" jack references outside journal history
- No OLED references remaining
- No old "2× encoder" references
- All cross-references consistent across files
