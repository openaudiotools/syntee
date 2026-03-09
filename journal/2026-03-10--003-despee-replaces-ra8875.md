# 003 — DESPEE replaces RA8875 display

**Date:** 2026-03-10
**Scope:** Full architectural change — RA8875 SPI display → external DESPEE UART module

## What changed

Replaced the integrated RA8875 SPI display controller with an external DESPEE display module (custom ESP32-S3 PCB + 4.3" 800×480 capacitive touch LCD). DESPEE connects via a 6-pin JST-PH header using Serial1 UART at 921600 baud.

## Pin reassignment

| Pin | Old assignment | New assignment |
|-----|---------------|----------------|
| 0 | Pad matrix row 0 | Serial1 RX (DESPEE TX → Teensy) |
| 1 | Pad matrix row 1 | Serial1 TX (Teensy → DESPEE RX) |
| 10 | RA8875 CS | Pad matrix row 0 (relocated) |
| 22 | RA8875 INT | DESPEE ESP32_EN |
| 37 | RA8875 RESET | DESPEE ESP32_GPIO0 |
| 41 | MIDI activity LED | Pad matrix row 1 (relocated) |

MIDI activity LED moved to 74HC595 shift register output 13 (4 spare outputs available).

## Key impacts

- **Display resolution:** 480×272 → 800×480 (upgrade)
- **GPIO budget:** Peripherals 12→16, GPIO available 30→26, spare ~3
- **Power budget:** ~460–480 mA → ~630–730 mA (still within USB-C 5V/1.5A)
- **SPI0 bus:** No longer shared — SD card gets exclusive access
- **CPU budget:** UI updates ~2% → <1% (offloaded to ESP32-S3)
- **New libraries:** esp-serial-flasher, ArduinoJson

## Files modified

| File | Change |
|------|--------|
| `README.md` | Updated display specs, added DESPEE module note |
| `docs/hardware.md` | Power budget, Key ICs (RA8875 → DESPEE module) |
| `docs/pin-mapping.md` | Full pin reassignment, GPIO budget update |
| `docs/firmware.md` | Libraries, CPU budget, Display Driver → Display Module section |
| `hardware/pcbs/main/README.md` | Board description, Key ICs, zones, diagram, placement rules |
| `hardware/pcbs/main/connections.md` | Display section → DESPEE 6-pin JST-PH, SD card exclusive, pad matrix |
| `hardware/pcbs/main/architecture.md` | Display Interface → Display Module, pad rows, SD card, MIDI LED |

## Context

Part of cross-repo effort to make DESPEE (openaudiotools/despee) the standard display component across all Open Audio Tools devices. MIXTEE and ABOUT repos updated in the same session.
