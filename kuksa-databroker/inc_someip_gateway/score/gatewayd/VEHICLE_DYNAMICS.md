# Vehicle Dynamics SOME/IP Bridge

`gatewayd` does not generate speed samples. `someipd` receives network events,
then `RemoteServiceInstance::forward_event` decodes their payload and sends
typed samples through an mw::com skeleton. This replaces the sample-producing
role of `vehicle-dynamics-example` for the configured remote instance.

## Payload Contract

For `vehicle::VehicleDynamicsService`, `speed` and `speedAck` use
`VehicleDynamicsSerializerConfig` in `score/config/mw_someip_config.json`.
The wire payload, excluding the 16-byte SOME/IP header, is exactly nine bytes:

- Bytes 0 through 7: IEEE-754 double in big-endian order.
- Byte 8: unsigned quality value.

For example, value `125.0`, quality `3` is `40 5f 40 00 00 00 00 00 03`.
The serializer reconstructs the native `double value; uint8_t quality;` layout
used by the example's `SpeedSample` and `SpeedAck`, including native padding.
It rejects payloads that are not exactly nine bytes. No unit conversion is
performed by this serializer.

`NullSerializerConfig` remains available for pre-serialized byte-buffer
services such as high beam. Those buffers contain a size prefix and are not
interchangeable with typed `SpeedSample` objects.

## Instance Routing

The current gateway configuration maps:

- Service1, instance ID 1: local mw::com events forwarded to SOME/IP.
- Service2, instance ID 2: incoming SOME/IP events published to mw::com.

The example producer and the existing Databroker demo select
`/Vehicle/Service1/Instance`. To consume incoming network speed without changing
gateway routing, the consumer must instead select `/Vehicle/Service2/Instance`
and use a matching mw::com manifest. Merely loading another manifest does not
change an instance specifier selected in consumer code.

Do not start the example producer and the gateway remote skeleton as providers
of the same mw::com service/instance. Coordinate application IDs and access
permissions in the provider and consumer manifests.

## Rebuild And Deploy

Build the gateway, serializer plugin, and binary configuration together:

```sh
bazel build --config=aarch64-linux \
  //score/gatewayd:gatewayd_example
```

Deploy the rebuilt `score_com_serializer.so` and `mw_someip_config.bin` along
with the gateway executable. Keep the mw::com service-instance manifest and
`someipd` network configuration consistent with these service and event IDs.
The sender must use the same nine-byte wire contract and eventgroup mapping.

Focused host checks:

```sh
bazel test --config=x86_64-linux \
  //score/serializer:null_serializer_test \
  //score/gatewayd:mw_com_config_schema_valid
```