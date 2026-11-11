# score_app: Vehicle High-Beam mw::com Showcase

## All-Signal Test App

`body_signal_test_app` is a separate, automatic round-trip test for all 44
configured body, door, and powertrain signals. It uses the existing route configuration and prints
`TX`, `RX`, `PASS`/`FAIL`, and an overall summary. The original vehicle menu
application remains unchanged.

After rebuilding and redeploying both archives, start the remote app normally,
then run this on the vehicle Pi:

```bash
VEHICLE_APP_MODE=signals ~/sdv-demo/run/start-vehicle.sh
```

Use only on an isolated test rig: the sweep sends actuator values and does
not restore previous values when interrupted. This mode skips KUKSA and the
legacy publisher to avoid a competing Tx service. A matching received value
confirms transport, not physical actuator operation. See the parent
[all-signal test instructions](../README.md#test-all-44-signals) for test
values, timeouts, expected output, and host checks.

For a stack already running without another body-signal Tx publisher:

```bash
cd ~/sdv-demo
./body_signal_test_app --configuration mw_com_config.json --routes signal_routes.json
```

## Original Vehicle App

This application publishes and consumes only the VSS signal
`Vehicle.Body.Lights.Beam.High.IsOn` through the SOME/IP gateway. Service,
instance, and member identifiers are transport configuration owned by `mw::com`
and the gateway; they are not part of the application signal interface.

The wire payload is exactly one byte: `0x00` for `false` and `0x01` for `true`.
The application rejects every other payload length or value.

The two directions use separate services so a received network state is never
republished back to the network:

| Direction | Service | Event | Instance |
| --- | --- | --- | --- |
| Vehicle app to network | `body_control_module_tx` (`0x4300`) | `0x8430` | `0x1000` |
| Network to Vehicle app | `body_control_module_rx` (`0x4300`) | `0x8431` | `0x1001` |

`gatewayd` consumes the Tx service through a `mw::com::GenericProxy` and exposes
the Rx service through a `mw::com::GenericSkeleton`. `someipd` owns the local
vSomeIP network stack.

The shared-memory binding is resolved as `service / instance / member`:
`/body_control_module_tx` or `/body_control_module_rx` / their configured instance
specifier / `high_beam_state`.

## Build

```bash
cd sdv-hack-demo
bash build-aarch64.sh
```

## Deployment

Use the packaged two-RPi workflow in the parent [SDV High-Beam Demo README](../README.md).
It builds the gateway components, creates both deployment archives, and starts
the vehicle and remote applications with the correct library paths and IP
configuration.

The Vehicle app publishes either boolean value. The bridge forwards it to the
remote sensor over UDP, and the sensor sends its latest value back through the
bridge, SOME/IP, and mw::com.

## Local host testing (no RPi required)

You can run the bridge and apps on your PC for fast iteration using localhost
UDP ports.

1. Create a local copy of the network config and set both IPs to `127.0.0.1`:

```bash
cp sdv-hack-demo/deploy/network.env sdv-hack-demo/deploy/network.env.local
# Edit values inside to:
# SDV_VEHICLE_IP=127.0.0.1
# SDV_REMOTE_IP=127.0.0.1
# SDV_BRIDGE_UDP_PORT=35000
# SDV_REMOTE_UDP_PORT=35001
nano sdv-hack-demo/deploy/network.env.local
source sdv-hack-demo/deploy/network.env.local
```

2. Build host binaries (x86_64):

```bash
bazel build --host_copt=-std=gnu11 //sdv-hack-demo/...
```

3. Run components in separate terminals (or background jobs):

Terminal A — bridge (vehicle-side):

```bash
bazel run //sdv-hack-demo/bridge:udp_bridge
```

Terminal B — remote sensor (simulated remote):

```bash
bazel run //sdv-hack-demo/remote_app:remote_app
```

Terminal C — vehicle app (foreground):

```bash
bazel run //sdv-hack-demo/vehicle_app:score_app
```

If `bazel run` target names differ, locate the binaries under `bazel-bin/`
and run them directly.

When the vehicle application is running, select a signal from its menu:

```text
1   High-beam; enter true or false
2   Speed; enter a numeric value
q   Quit
```

Speed uses the vehicle-dynamics example payload: an 8-byte `double` followed by
the 1-byte quality value `3`. The remote endpoint returns the same payload as
`Vehicle.speedAck`.

## TODO (local development)
- Add `sdv-hack-demo/run-local.sh` to launch bridge + remote + vehicle and
	capture logs.
- Provide `remote_app/README.md` with host-run instructions.
- Add a lightweight host-only integration test asserting message exchange.