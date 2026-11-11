<!--
Copyright (c) 2026 Contributors to the Eclipse Foundation

See the NOTICE file(s) distributed with this work for additional
information regarding copyright ownership.

This program and the accompanying materials are made available under the
terms of the Apache License Version 2.0 which is available at
https://www.apache.org/licenses/LICENSE-2.0

SPDX-License-Identifier: Apache-2.0
-->

# COM API Example - Vehicle Monitor Application

## Body and Door C++ Interfaces

The C++ definitions in [com-api-gen/vehicle_gen.h](com-api-gen/vehicle_gen.h)
also expose all 37 configured body and door signals through `HighBeamTx` and
`VehicleHighBeamRx`. The existing names are retained for compatibility.
For example, `horn_state`, `rear_trunk_position_state`, and
`front_wiping_mode_state` are available as typed events on both interfaces.
The vehicle-dynamics Rust application's speed behavior is unchanged.

Use the header-only Bazel target `:vehicle_gen_types` in the `com-api-gen`
package for C++ consumers. The existing `:vehicle_gen_cpp` target additionally
exports the interfaces to the Rust FFI registry.

| Interface | Instance Specifier | Sample Layout |
| --- | --- | --- |
| `HighBeamTx` | `/vehicle_high_beam/local_tx` | `HighBeamCommand` for boolean/uint8, `BodySignalText` for strings |
| `VehicleHighBeamRx` | `/vehicle_high_beam/network_rx` | `HighBeamState` for booleans, `HighBeamCommand` for uint8, `BodySignalText` for strings |

`HighBeamState.value` is a native boolean. Pre-serialized samples contain
`size` and `data`: set `size = 1` for boolean/uint8 values, or the UTF-8 byte
count (at most 32, no terminating NUL) for `BodySignalText`. Boolean wire
values must be `0` or `1`. Position, intensity, and fluid-level values are
unsigned percentages. The buffer payload starts at byte offset 16, matching
the gateway serializer's shared-memory layout.

The example's [etc/mw_com_config.json](etc/mw_com_config.json) contains the
matching Tx/Rx services and event IDs. From the integration workspace root,
run `python3 sdv-hack-demo/configure_signals.py --write` to synchronize this
config and the C++ event list from the body-signal manifest. This adds service
definitions, not automatic body-signal publishers or application controls.

From the `communication` workspace, run the interface regression tests:

```bash
bazel test //score/mw/com/example/vehicle-dynamics-example/com-api-gen:body_signal_interface_test
```

## BodyControlModuleService Integration

The `body-control-module-example` target uses the BodyControl service contract
and a separate LoLa instance for datapoints and actuator feedback. Start the
KUKSA composition from `kuksa-databroker/`:

```bash
cargo run -p databroker-mw-com
```

Then, from the `communication` workspace, run:

```bash
bazel run //score/mw/com/example/vehicle-dynamics-example:body-control-module-example -- --run-mode both
```

The producer publishes washer, rain, door-child-lock, and vehicle-speed
datapoints. The consumer subscribes to horn actuation. The existing
`vehicle-dynamics-example` target and gateway service instances remain separate.

## Overview

This example application demonstrates the COM (Communication Middleware) API capabilities using a Vehicle Monitor scenario. It showcases producer-consumer communication patterns with the Lola runtime, featuring both synchronous and asynchronous operations.

## Use Case

The application simulates a **Vehicle Monitor System** that tracks two types of events:
- **Tire Pressure Data**: Monitoring tire pressure readings
- **Exhaust Data**: Monitoring exhaust system information

The producer publishes vehicle data, while the consumer subscribes to and processes these events.

## Library Components

The `com-api-example-lib` library is organized into the following modules:

1. **Producer Module** (`producer.rs`)
   - Creates and offers vehicle data services

2. **Consumer Module** (`consumer.rs`)
   - Subscribes to vehicle events and handles them via both synchronous and asynchronous APIs

## Main Application

The main application (`main.rs`) demonstrates:

### Multi-threaded Architecture
- **Producer Thread**: Sends vehicle data at regular intervals
- **Consumer Thread**: Receives and processes data independently

### Async Operations
- We are using `futures::executor::block_on()` for executing async APIs in this sample application,
  user can use any other async executor or runtime based on requirement.

## Command-Line Options

### Main Application

```bash
bazel run //score/mw/com/example/com-api-example:com-api-example -- [OPTIONS]

Options:
  -r, --run-mode <MODE>        Which roles to start [default: both]
                               [possible values: both, producer, consumer]

  -e, --example-type <TYPE>    Execution mode [default: sync]
                               [possible values: sync, async-service-discovery,
                                async-receive, async-receive-with-timeout, streaming]

  -s, --service-instance-manifest <PATH>
                               Path to service configuration JSON
                               [default: ./score/mw/com/example/com-api-example/etc/mw_com_config.json]
```

### Examples

```bash
# Run with default sync mode (both producer and consumer)
bazel run //score/mw/com/example/com-api-example:com-api-example

# Run only the producer
bazel run //score/mw/com/example/com-api-example:com-api-example -- -r producer

# Run only the consumer
bazel run //score/mw/com/example/com-api-example:com-api-example -- -r consumer

# Test async receive with timeout/cancellation
bazel run //score/mw/com/example/com-api-example:com-api-example -- -e async-receive-with-timeout

# Run only consumer with async receive with timeout feature
bazel run //score/mw/com/example/com-api-example:com-api-example -- -r consumer -e async-receive-with-timeout

# Test streaming with custom config
bazel run //score/mw/com/example/com-api-example:com-api-example -- \
  -e streaming \
  -s ./path/to/custom/config.json
```

## Integration Tests

### Tokio crate based Integration Tests

A separate test target (`tests_using_tokio_runtime.rs`) provides comprehensive integration tests using the **Tokio async runtime**. These tests are independent of the main application and use the `com-api-gen` generated types together with the `com_api` runtime, service discovery, and subscription APIs directly.

### Running Tests

```bash
# Run all integration tests
bazel test //score/mw/com/example/com-api-example:com-api-example-tokio-integration-test

# Run specific test
bazel test //score/mw/com/example/com-api-example:com-api-example-tokio-integration-test \
  --test_filter=receive_and_send_using_multi_thread

# Run with verbose output for debugging
bazel test //score/mw/com/example/com-api-example:com-api-example-tokio-integration-test \
  --test_output=all --test_arg=--nocapture
```

## Note
- Running the application without any arguments uses the default configuration, executing all synchronous
  APIs with both producer and consumer in the same process.
- When running only the consumer in synchronous mode, start the producer first because in this example app
  synchronous service discovery operation does not poll and will fail if no service is already offered.
- When running the consumer with async API modes, asynchronous service discovery blocks until the producer
  offers the service. Except for `async-receive-with-timeout`, all other async receive modes block until data arrives and all iterations complete.

## Further Reading
- [COM API Documentation](../../impl/rust/com-api/README.md)

---
