# Board Zoning Orientation Flip

**Date:** 2026-03-08
**Scope:** `hardware/pcbs/main/README.md` — Board Zoning section

---

## Summary

Flipped the board zoning orientation so that north = top panel (user-facing) and south = rear. The noise isolation principle is unchanged — audio and Ethernet remain on opposite edges.

## What Changed

The original zoning had north = digital/noisy (Ethernet, USB Host) and south = analog/quiet (audio jacks, MIDI). After clarifying that north should represent the top panel (where the user interacts with the device), the map was flipped:

- **North edge (top panel):** Audio jacks (4× 1/4" TS), MIDI IN/OUT (3.5mm TRS), USB power entry
- **Near-north:** OPA1678 ×2, 6N138 optocoupler, BAT54 ESD diodes, filter passives
- **Center-north:** ADP7118 LDO, AK4619VN codec (analog pins face north toward jacks), encoder/display headers
- **Center:** Teensy 4.1 on socket headers (I2S/I2C north end faces codec, ETH/USB pads south end faces rear)
- **South edge (rear):** RJ45 MagJack (Ethernet), USB-A Host, AP2553

## Key Placement Rules Updated

- Rule 1: RJ45 at south, audio jacks at north (was reversed)
- Rule 2: Codec analog pins face north toward jacks, digital pins face south toward Teensy (was reversed)
- All other rules (MCLK spacing, op-amp proximity, analog power locality, no digital routing through analog zone) unchanged

## Files Modified

| File | Changes |
|------|---------|
| `hardware/pcbs/main/README.md` | ASCII zone map flipped, intro paragraph updated, key placement rules 1–2 directional references swapped |
