# Design Review Fixes — Senior EE Assessment

**Date:** 2026-03-08
**Scope:** Full system architecture review, all documentation updated

---

## Summary

Implemented fixes from a comprehensive design review covering 17 items across the SynTee documentation. All 5 critical issues resolved, all significant concerns addressed, all minor items documented.

## Critical Fixes (Items 1–5)

### 1. MIDI OUT Circuit — Fixed for 3.3V Logic
- **Problem:** Resistor topology was non-standard for 3.3V sources. Source and sink pins were swapped relative to MMA CA-033.
- **Fix:** Corrected to standard 3.3V MIDI OUT per MMA CA-033: 3.3V → 10Ω → Tip (source), TX → 33Ω → Ring (sink).
- **Files:** `hardware/pcbs/main/architecture.md`, `connections.md`, `docs/hardware.md`

### 2. USB Host Overcurrent Protection — Added
- **Problem:** No protection on USB host VBUS output. A shorted USB device could damage the Teensy.
- **Fix:** Added AP2553 USB power switch IC (~500 mA limit, thermal shutdown, enable control).
- **Files:** `hardware/pcbs/main/architecture.md`, `connections.md`, `README.md`, `docs/hardware.md`

### 3. Codec Reset Sequencing — Specified
- **Problem:** AK4619VN PDN pin control and power-up sequence were undefined.
- **Fix:** Assigned GPIO pin 2 for PDN control. Documented full reset sequence (power → wait → MCLK → wait → release PDN → wait → I2C config). Added firmware init code example.
- **Files:** `hardware/pcbs/main/architecture.md`, `docs/pin-mapping.md`, `docs/firmware.md`

### 4. Input Overvoltage Protection — Added
- **Problem:** BAT54 clamp diodes had no series resistance — excessive clamp current at high input levels.
- **Fix:** Added 1 kΩ series resistor before BAT54 clamp diodes (to 3.3V_A and GND). Limits clamp current to ~4.5 mA at +20 dBu. Resistor doubles as part of anti-alias RC filter.
- **Files:** `hardware/pcbs/main/architecture.md`, `connections.md`, `docs/hardware.md`

### 5. Ethernet Coupling Caps — Removed
- **Problem:** External 0.1 µF coupling caps between Teensy and MagJack created triple-coupled path (PHY on-chip + caps + MagJack magnetics).
- **Fix:** Direct connection from Teensy bottom pads to MagJack via short, 100Ω impedance-controlled differential traces. Follows PJRC reference design.
- **Files:** `hardware/pcbs/main/connections.md`, `docs/pin-mapping.md`

## Significant Fixes (Items 6–10)

### 6. Op-Amp Selection — Finalized OPA1678
- **Problem:** NJM4580 listed as candidate but requires bipolar supply — cannot operate on 3.3V single-supply.
- **Fix:** Selected OPA1678 exclusively. Documented single-supply biasing at AVDD/2 (1.65V virtual ground).
- **Files:** `hardware/pcbs/main/architecture.md`, `README.md`, `docs/hardware.md`

### 7. Filter Design — Specified
- **Problem:** Anti-alias and reconstruction filters mentioned but no component values or cutoff frequencies.
- **Fix:** Input: 1st-order RC (1 kΩ + 3.3 nF = ~48 kHz). Output: 2nd-order Sallen-Key Butterworth (R=1 kΩ, C1=3.9 nF, C2=1.8 nF = ~40 kHz, Q≈0.7).
- **Files:** `hardware/pcbs/main/architecture.md`, `connections.md`

### 8. Ground Plane Strategy — Expanded
- **Problem:** Grounding strategy was too vague for a mixed-signal board with codec.
- **Fix:** Added detailed mixed-signal grounding section: codec analog return paths, single-point merge, placement guidance, decoupling via placement. Reference to AK4619VN datasheet layout.
- **Files:** `docs/pcb-design-rules.md`

### 9. 1/4" Jacks — Acknowledged
- **Problem:** 1/4" jacks are physically large for a compact module.
- **Fix:** Added notes in connections.md acknowledging size impact and suggesting 3.5mm TRS as alternative. Decision deferred to enclosure design phase.
- **Files:** `hardware/pcbs/main/connections.md`

### 10. USB Host Routing — Documented
- **Problem:** No guidance on routing USB differential pairs from Teensy bottom pads.
- **Fix:** Added 90Ω differential pair requirement, placement guidance (Teensy near board edge), and differential pair table in PCB design rules.
- **Files:** `hardware/pcbs/main/connections.md`, `docs/pcb-design-rules.md`

## Minor Fixes (Items 11–17)

### 11. Display TBD — Added Pin-Lock Note
- Note added that display choice affects SPI/I2C pin allocation and should be finalized before layout.

### 12. Polyphony Target — Clarified to 8 Voices
- Changed from vague "8–16" to firm "8 voices" target with note to benchmark before expanding.

### 13. PTP Accuracy — Corrected to ~50–200 µs
- Updated from optimistic "~10–100 µs" to realistic "~50–200 µs" for software PTP.

### 14. Enclosure — No action (out of scope for this review)

### 15. QFN Thermal — OK, no changes needed

### 16. Test Points — Added
- 7 test points specified: 5V, 3.3V_A, MCLK, SDA, SCL, MIDI IN data, PDN.

### 17. Firmware Watchdog — Added
- Documented hardware watchdog (WDOG1) requirement, timeout guidance, and integration with main loop.

## Files Modified

| File | Changes |
|------|---------|
| `hardware/pcbs/main/architecture.md` | MIDI OUT fix, codec PDN/reset, op-amp selection, filter design, power protection, input protection, test points |
| `hardware/pcbs/main/connections.md` | MIDI OUT fix, Ethernet fix, USB host protection, audio I/O updates, jack size notes |
| `hardware/pcbs/main/README.md` | Key ICs list updated (OPA1678, AP2553, PDN, no coupling caps) |
| `docs/hardware.md` | MIDI OUT fix, AP2553 added, OPA1678 added, input protection resistors |
| `docs/pin-mapping.md` | Pin 2 assigned to PDN, Ethernet coupling caps removed, pin counts updated |
| `docs/firmware.md` | Codec init sequence, watchdog, polyphony target, PTP accuracy |
| `docs/pcb-design-rules.md` | Mixed-signal grounding detail, differential pair routing table |
| `docs/network-connectivity.md` | PTP accuracy corrected |
