<!--
Copyright (c) 2026 Contributors to the Eclipse Foundation

SPDX-License-Identifier: Apache-2.0
-->

# SDV Demo Applications

This directory contains the vehicle menu, UDP bridge, remote application,
signal-route tester, and ARM64 build/deployment scripts.

Use the [main operator guide](../README.md) for the complete workflow:

- [Prerequisites and toolchain](../README.md#prepare-the-host-machine-and-rpi-hw)
- [Build and package](../README.md#build-and-package)
- [Deployment and network settings](../README.md#deploy-and-configure)
- [Native stack startup](../README.md#start-the-demo)
- [SDV Runtime container](../README.md#setup-and-run-the-sdv-runtime)
- [Playground use case and GPIO](../README.md#playground-use-case-and-gpio)
- [Stopping and replacing a deployment](../README.md#stop-or-replace-a-deployment)
- [Troubleshooting](../README.md#troubleshooting)
- [Signal configuration and development](../README.md#signal-configuration-and-development)
- [Supported signals](../README.md#supported-signals)

Builds require the full repository, not this directory alone.

## Source Map

| Location | Purpose |
| --- | --- |
| [vehicle_app](vehicle_app/) | Vehicle menu and body-signal round-trip tester |
| [bridge](bridge/) | Bidirectional local SOME/IP / remote UDP relay |
| [remote_app](remote_app/) | Remote UDP state, menus, GPIO, and publication sweep |
| [include/high_beam_udp_protocol.h](include/high_beam_udp_protocol.h) | Shared 46-byte UDP frame with a 32-byte payload limit |
| [config/body_signals.json](config/body_signals.json) | Manifest for the 44 body/powertrain signal routes |
| [signal_routes.json](signal_routes.json) | Generated numeric routes, including vehicle dynamics |
| [configure_signals.py](configure_signals.py) | Signal configuration synchronizer |
| [gateway_provider_codegen.py](gateway_provider_codegen.py) | KUKSA mappings and typed mw::com binding generation |
| [serializer](serializer/) | Gateway serializer shim |
| [deploy](deploy/) | Device launchers and default network settings |
| [sdv-runtime](sdv-runtime/) | SDK overlay and separate model patch |

See [ARCHITECTURE.md](../ARCHITECTURE.md) for service identifiers, payload
layouts, provider flow, and the separate standalone body-control profile.
