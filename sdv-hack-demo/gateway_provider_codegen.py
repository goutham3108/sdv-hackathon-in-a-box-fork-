# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

"""Generate KUKSA bindings for the existing gateway's Tx/Rx contract."""

import json


def gateway_provider_documents(root):
    signals = json.loads((root / "sdv-hack-demo/config/body_signals.json").read_text())["signals"]
    mappings = []
    for index, signal in enumerate(signals, start=1):
        event = signal.get("eventName", signal["name"] + "_state")
        signal_type = signal.get("type")
        service = "/body_control_module_tx" if signal_type == "actuator" else "/body_control_module_rx"
        instance = "local_tx" if signal_type == "actuator" else "network_rx"
        direction = {
            "sensor": "datapoint",
            "actuator": "actuator",
        }.get(signal_type, "bidirectional")
        mapping = {
            "signal_id": index,
            "vss_path": signal["apiName"],
            "datatype": {
                "boolean": "bool",
                "uint8": "uint8",
                "int16": "int16",
                "float": "float",
                "float_array": "float_array",
                "string": "string",
            }[signal["datatype"]],
            "service": service,
            "instance": instance,
            "member": event,
            "direction": direction,
            "cycle_time_ms": 100,
            "required_quality": "valid",
        }
        broker_types = {
            "Vehicle.Body.Lights.Brake.IsActive": "string",
            "Vehicle.OBD.OilTemperature": "float",
        }
        mapping["datatype"] = broker_types.get(signal["apiName"], mapping["datatype"])
        if signal["datatype"] == "uint8":
            mapping.update({"min": 0, "max": 100, "unit": "percent"})
        if signal_type not in {"sensor", "actuator"}:
            mapping["actuation_binding"] = {
                "service": "/body_control_module_tx",
                "instance": "local_tx",
                "member": event,
            }
        if signal.get("unit"):
            mapping["unit"] = signal["unit"]
        if signal.get("maxElements"):
            mapping["max_elements"] = signal["maxElements"]
        mappings.append(mapping)
    mappings.append(
        {
            "signal_id": len(signals) + 1,
            "vss_path": "Vehicle.Speed",
            "datatype": "float",
            "service": "VehicleDynamicsService",
            "instance": "network_rx",
            "member": "speed",
            "direction": "datapoint",
            "unit": "km/h",
            "cycle_time_ms": 100,
        }
    )
    return {
        root / "sdv-hack-demo/config/gateway_provider_config.json": {
            "provider_name": "gateway_mw_com_provider",
            "queue_capacity": 256,
            "stale_after_ms": 1000,
            "mappings": mappings,
        }
    }


def gateway_provider_sources(root):
    signals = json.loads((root / "sdv-hack-demo/config/body_signals.json").read_text())["signals"]
    metadata = ["use crate::generated::GenericMwComElement;", "pub const GATEWAY_BINDINGS: &[GenericMwComElement] = &["]
    runtime = []
    for direction, interface, instance, specifier in (
        ("tx", "HighBeamTx", "local_tx", "/body_control_module/local_tx"),
        ("rx", "VehicleHighBeamRx", "network_rx", "/body_control_module/network_rx"),
    ):
        runtime.append(f'score_com::interface!(interface {interface} {{ Id = "{interface}",')
        for signal in signals:
            event = signal.get("eventName", signal["name"] + "_state")
            kind = signal["datatype"]
            sample = {
                "string": "BodySignalText",
                "int16": "BodySignalPayload",
                "float": "BodySignalPayload",
                "float_array": "BodySignalPayload",
            }.get(kind, "HighBeamCommand")
            if direction == "rx" and kind == "boolean":
                sample = "HighBeamState"
            runtime.append(f"    {event}: Event<{sample}>,")
            size, alignment = {
                "HighBeamState": (1, 1),
                "HighBeamCommand": (32, 16),
                "BodySignalText": (48, 16),
                "BodySignalPayload": (48, 16),
            }[sample]
            metadata.append(
                f'GenericMwComElement {{ service_name: "/body_control_module_{direction}", '
                f'instance_name: "{instance}", instance_specifier: "{specifier}", '
                f'member_name: "{event}", element_kind: "event", payload_type: "{sample}", '
                f"sample_size: {size}, sample_alignment: {alignment}, has_serialized_format: false }},"
            )
        runtime.append("});")
    metadata.append(
        'GenericMwComElement { service_name: "VehicleDynamicsService", instance_name: "network_rx", '
        'instance_specifier: "/Vehicle/Service2/Instance", member_name: "speed", element_kind: "event", '
        'payload_type: "SpeedSample", sample_size: 16, sample_alignment: 8, has_serialized_format: false },'
    )
    metadata.append("];")
    runtime.extend(
        [
            'score_com::interface!(interface VehicleDynamicsService { Id = "VehicleDynamicsService",',
            "speed: Event<SpeedSample>, speedAck: Event<SpeedAck>, });",
            "impl GatewayScoreRuntimeAdapter {",
            "fn subscribe_body(&mut self, binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {",
            "let consumer = self.runtime()?.find_service::<VehicleHighBeamRxInterface>(",
            "FindServiceSpecifier::Specific(Self::instance(binding)?)).get_available_instances()",
            '.map_err(|error| Self::error("body discovery", error))?.into_iter().next()',
            '.ok_or_else(|| MwComProviderError::Transport("gateway body Rx service is unavailable".into()))?',
            '.build().map_err(|error| Self::error("body consumer", error))?;',
            "let subscription = match binding.member_name {",
        ]
    )
    for signal in signals:
        event = signal.get("eventName", signal["name"] + "_state")
        variant = {
            "boolean": "Bool",
            "uint8": "Byte",
            "int16": "Payload",
            "float": "Payload",
            "float_array": "Payload",
            "string": "Text",
        }[signal["datatype"]]
        datatype_arg = f', "{signal["datatype"]}"' if variant == "Payload" else ""
        runtime.append(
            f'"{event}" => GatewaySubscription::{variant}(consumer.{event}.subscribe(4)'
            f'.map_err(|error| Self::error("body subscribe", error))?{datatype_arg}),'
        )
    runtime.extend(
        [
            '_ => return Err(MwComProviderError::Transport("unknown gateway body event".into())),',
            "};",
            "self.subscriptions.push(SubscriptionEntry::new(binding, subscription)); Ok(()) }",
            "fn send_body(&mut self, binding: ScoreBinding<'_>, "
            "value: &MwComValue) -> Result<(), MwComProviderError> {",
            "let producer = self.tx.as_ref().ok_or_else(|| "
            'MwComProviderError::Transport("gateway Tx is disabled".into()))?;',
            "match binding.member_name {",
        ]
    )
    for signal in signals:
        event = signal.get("eventName", signal["name"] + "_state")
        sample = {
            "string": "BodySignalText",
            "int16": "BodySignalPayload",
            "float": "BodySignalPayload",
            "float_array": "BodySignalPayload",
        }.get(signal["datatype"], "HighBeamCommand")
        runtime.append(
            f'"{event}" => producer.{event}.send({sample}::encode(value, "{signal["datatype"]}")?)'
            '.map_err(|error| Self::error("gateway publish", error)),'
        )
    runtime.extend(
        [
            '_ => Err(MwComProviderError::Transport("unknown gateway Tx event".into())),',
            "} } }",
        ]
    )
    generated = root / "kuksa-databroker/mw_com_provider/generated"
    return {
        generated / "gateway_bindings.rs": "\n".join(metadata) + "\n",
        generated / "gateway_runtime.rs": "\n".join(runtime) + "\n",
    }
