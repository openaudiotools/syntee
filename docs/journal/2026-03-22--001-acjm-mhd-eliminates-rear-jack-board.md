# 001 — Replace 8x 3.5mm TS jacks and rear jack board with 4x Amphenol ACJM-MHD dual 1/4" mono jacks

**Date:** 2026-03-22
**Scope:** hardware/pcbs/main, docs/hardware.md, README.md

## Summary

Replaced the planned 8x individual 3.5mm TS audio jacks with 4x Amphenol ACJM-MHD dual stacked 1/4" mono (TS) jacks. Each ACJM-MHD carries one stereo pair (top = L, bottom = R), mounting directly to the main PCB. This eliminates the separate rear jack board that was planned to carry the lower row of jacks and MIDI connectors, reducing the design from two boards to one (plus external DESPEE and PHONEE modules).

## What changed

| File | Change |
|------|--------|
| `hardware/pcbs/main/connections.md` | Audio Output/Input sections rewritten for ACJM-MHD dual jacks (2 units per section instead of 4 individual jacks) |
| `hardware/pcbs/main/README.md` | Removed "separate rear jack board" reference; updated analog zone to ACJM-MHD; updated board zoning diagram; added ACJM-MHD to Key ICs list |
| `docs/hardware.md` | Board dimensions line updated — removed "additional rear jack board" mention |
| `hardware/pcbs/main/architecture.md` | Audio input protection circuit updated from "3.5mm jack" to "1/4" TS jack" |
| `README.md` | Updated hardware features: removed "rear jack board" from modularity description; changed "3.5mm jacks" to "1/4" TS jacks" |

### Part selection: Amphenol ACJM-MHD

Evaluated three candidates:

| Part | Type | Contacts | Cycles | Switch contacts | Price |
|------|------|----------|--------|-----------------|-------|
| Neutrik NSJ12HF-1 | Dual 1/4" stereo (TRS) | Silver-plated | 10,000 | Yes (2x3) | ~$2.81 |
| Amphenol ACJM-MHD | Dual 1/4" mono (TS) | Tin-plated | 10,000 | No | ~$2.26 |
| Amphenol ACJS-MHDEM | Dual 1/4" stereo (TRS) | Tin-plated | 1,000 | No | unclear |

Chose ACJM-MHD: mono matches the TS use case exactly, simpler wiring, same durability as Neutrik, good availability.

### ACJM-MHD dimensions (from datasheet drawings)

- Width: 16.0mm
- Height (dual unit): 31.75mm (19.05mm center-to-center between jacks)
- Depth behind panel: 33.5mm
- PCB pin protrusion: 4.50mm typical

### Panel fit verification

140mm panel width vs. 4x ACJM-MHD (16mm each) + 2x 3.5mm TRS MIDI (~12mm each) = 88mm total, leaving 52mm for gaps and margins.

## Context

The original design deliberated placing an extra board for the lower row of 1/4" TS and TRS MIDI jacks on the back panel, with upper jacks on the main board. The ACJM-MHD's dual stacked form factor merges both rows into single PCB-mount units, making the separate board unnecessary. MIDI 3.5mm TRS jacks remain on the same (north/top) edge of the main board.

The layout design files (`.afdesign`, `.svg`, `.png`) still show the old arrangement with individual jacks and will need manual update.
