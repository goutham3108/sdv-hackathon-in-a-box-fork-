<!--
/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
-->

# Folder Structure

This document describes the recommended publication structure for the `mw_com_provider` crate.

```text
mw_com_provider/
├── Cargo.toml
├── README.md
├── build.rs
├── config/
├── src/
│   ├── lib.rs
│   ├── provider.rs
│   ├── transport.rs
│   ├── mapper.rs
│   ├── config.rs
│   ├── lifecycle.rs
│   ├── quality.rs
│   ├── bindings.rs
│   ├── error.rs
│   ├── native_bridge.rs
│   └── score_ffi_transport.rs
└── docs/
    ├── folder-structure.md
    ├── low-level-design.md
    ├── publication-checklist.md
    ├── mw_com_provider_lld.drawio
    ├── generation-pipeline.md
    ├── qm-boundary.md
    ├── samples.md
    ├── startup.md
    └── adr/
```

## Root Files

| Path | Purpose |
| --- | --- |
| `Cargo.toml` | Crate metadata, features (`score-transport`, `ffi-bindings`, `mock-transport`), dependencies, and build script. |
| `README.md` | Public entry point for users and reviewers. Keep overview, build commands, feature selection, and links to detailed docs here. |
| `build.rs` | Build-time hook that loads the Rust metadata generated under `generated/`. |

## Source Layout

| Path | Purpose |
| --- | --- |
| `src/lib.rs` | Public crate exports and generated binding include. Conditionally exports transport implementations. |
| `src/provider.rs` | Main provider runtime, Databroker registration, receive loop, and actuation callback. |
| `src/transport.rs` | `MwComTransport` trait definition and `MockMwComTransport` implementation. |
| `src/native_bridge.rs` | Generic C FFI abstraction (`GenericMwComNativeTransport`) for native middleware implementations. |
| `src/score_ffi_transport.rs` | Type-safe Rust wrapper (`ScoreTransport`) around SCORE's native Rust API. Uses Consumer/Subscriber/Subscription. |
| `src/mapper.rs` | Bidirectional conversion between `MwComMessage` and Databroker `EntryUpdate` or `ActuationChange`. |
| `src/config.rs` | JSON configuration model and validation for VSS-to-`mw::com` mappings. |
| `src/lifecycle.rs` | Reconnect configuration and backoff calculation. |
| `src/quality.rs` | Signal quality enum used for incoming sample qualification. |
| `models/vehicle_dynamics.json` | Service definition model consumed by the external code generator. |
| `templates/reference-codegen/` | Templates used by the external code generator. |
| `src/bindings.rs` | Small wrapper around generated binding metadata. |
| `src/error.rs` | Provider error model. |

## Documentation Layout

| Path | Purpose |
| --- | --- |
| `docs/low-level-design.md` | Main LLD with block and sequence diagrams, transport implementations, and dataflow. |
| `docs/mw_com_provider_lld.drawio` | Editable Draw.io source for the LLD diagrams. |
| `docs/samples.md` | Sample service definition model and generated mapping JSON. |
| `docs/startup.md` | Databroker startup integration patterns. |
| `docs/adr/` | Architecture decision records including: provider architecture, trait boundary design, FFI vs Rust API strategy. |

## Diagram Placement

The editable diagram source is kept in [mw_com_provider_lld.drawio](mw_com_provider_lld.drawio). Markdown documents reference it and include Mermaid previews so architecture information remains visible in plain repository viewers.

Recommended usage:

- Link the Draw.io file from `README.md` for maintainers who want editable diagrams.
- Keep rendered sequence/block previews in `docs/low-level-design.md` for quick review.
- Add exported PNG or SVG files only if the target publishing system cannot render Mermaid or open Draw.io files.