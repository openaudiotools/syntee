# 001 — Reframe SynTee as DSP module with loadable functions

**Date:** 2026-03-21
**Scope:** README, docs, CLAUDE.md — project identity and architecture framing

## Summary

Reframed SynTee from "synthesizer on a single PCB" to a DSP module with a two-layer firmware architecture. The base firmware layer exposes hardware (codec, MIDI, network, controls); a swappable functional layer defines device behavior (synth, effects, sampler, etc.). The primary/default function remains a MIDI-controlled synthesizer. Also corrected the "single board" claim — the device is modular: main board + rear jack board + DESPEE display module.

## What changed

| File | Change |
|------|--------|
| `README.md` | Subtitle → "DSP module for synths, effects, samplers, and more." Intro paragraph explains two-layer firmware. "Single-board design" bullet → "Modular hardware." Architecture section notes diagram shows synth mode. Signal flow label → "DSP Engine." Repo tree comment updated. |
| `docs/firmware.md` | Added two-layer architecture description. "Core function" → "Default function." |
| `docs/hardware.md` | "synthesizer outputs" → "audio outputs." Board dimensions note updated to mention rear jack board. |
| `hardware/pcbs/main/README.md` | "Single board for the entire SynTee synthesizer" → "Main board for SynTee" with jack board and DESPEE mentioned. |
| `CLAUDE.md` | Project overview and architecture summary updated to DSP module framing. |

## Context

This is a fundamental identity shift. SynTee is not a single-purpose synthesizer — it's a DSP platform whose function is defined by firmware. The rear jack board is a new addition to accommodate two layers of back-panel connectors. The jack board's exact role (passive routing vs. active circuitry) is not yet defined. Other docs (`docs/firmware.md` synth engine sections, `docs/network-connectivity.md`) still contain synth-specific language in their detailed sections — that's intentional, as those sections describe the synth function specifically.
