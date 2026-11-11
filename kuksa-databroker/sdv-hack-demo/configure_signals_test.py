"""Check body signal registrations across the deployed transport configuration."""

import unittest

from configure_signals import ROOT, body_signal_declarations, configure, read_json
from gateway_provider_codegen import gateway_provider_documents, gateway_provider_sources


class SignalConfigurationTest(unittest.TestCase):
    def test_gateway_provider_uses_existing_transport_contract(self):
        document = next(iter(gateway_provider_documents(ROOT).values()))
        assert len(document["mappings"]) == 45
        for mapping in document["mappings"][:-1]:
            if mapping["vss_path"] == "Vehicle.Powertrain.Transmission.PerformanceMode":
                assert mapping["service"] == "/body_control_module_tx"
                assert mapping["instance"] == "local_tx"
                assert mapping["direction"] == "actuator"
                assert "actuation_binding" not in mapping
                continue
            assert mapping["service"] == "/body_control_module_rx"
            if mapping["vss_path"] in {
                "Vehicle.Powertrain.CombustionEngine.IsRunning",
                "Vehicle.OBD.OilTemperature",
                "Vehicle.Powertrain.TractionBattery.CellVoltage.CellVoltages",
                "Vehicle.Acceleration.Vertical",
                "Vehicle.Chassis.Brake.PedalPosition",
                "Vehicle.Powertrain.FuelSystem.IsEngineStopStartEnabled",
            }:
                assert mapping["direction"] == "datapoint"
                assert "actuation_binding" not in mapping
            else:
                assert mapping["direction"] == "bidirectional"
                assert mapping["actuation_binding"]["service"] == "/body_control_module_tx"
                assert mapping["member"] == mapping["actuation_binding"]["member"]
        sources = "\n".join(gateway_provider_sources(ROOT).values())
        assert 'Id = "HighBeamTx"' in sources
        assert 'Id = "VehicleHighBeamRx"' in sources
        assert "low_beam_state" not in sources

    def setUp(self):
        self.demo = ROOT / "sdv-hack-demo"
        self.gateway = ROOT / "inc_someip_gateway"
        self.signals = read_json(self.demo / "config/body_signals.json")["signals"]

    def test_all_requested_signals_have_unique_routes(self):
        assert len(self.signals) == 44
        for key in ("apiName", "name", "vehicleEvent"):
            assert len({signal[key] for signal in self.signals}) == 44
        assert "Vehicle.Cabin.Door.Row2.PassengerSide.IsOpenx" not in {signal["apiName"] for signal in self.signals}
        routes = read_json(self.demo / "signal_routes.json")["routes"]
        wire_ids = set()
        expected_types = {
            "Vehicle.Powertrain.CombustionEngine.IsRunning": "boolean",
            "Vehicle.OBD.OilTemperature": "int16",
            "Vehicle.Powertrain.TractionBattery.CellVoltage.CellVoltages": "float_array",
            "Vehicle.Acceleration.Vertical": "float",
            "Vehicle.Chassis.Brake.PedalPosition": "uint8",
            "Vehicle.Powertrain.FuelSystem.IsEngineStopStartEnabled": "boolean",
            "Vehicle.Powertrain.Transmission.PerformanceMode": "string",
        }
        for signal in self.signals:
            route = next(item for item in routes if item["name"] == signal["name"])
            assert route["apiName"] == signal["apiName"]
            assert route["datatype"] == signal["datatype"]
            if signal["apiName"] in expected_types:
                assert signal["datatype"] == expected_types[signal["apiName"]]
            if signal["datatype"] == "float_array":
                assert route["maxPayloadSize"] == 32
                assert route["maxElements"] == 8
            assert route["remoteEvent"] == route["vehicleEvent"] + 1
            assert route["vehicleEvent"] >= 32768
            assert route["remoteEvent"] <= 65535
            for direction in ("vehicle", "remote"):
                key = (route["service"], route[direction + "Instance"], route[direction + "Event"])
                assert key not in wire_ids
                wire_ids.add(key)

    def test_each_route_has_matching_daemon_and_network_events(self):
        configs = [
            read_json(self.gateway / "score/config/mw_someip_config.json"),
            read_json(self.demo / "config/mw_someip_config.json"),
        ]
        lola = read_json(self.gateway / "score/gatewayd/etc/mw_com_config.json")
        network = read_json(self.gateway / "tests/integration/vsomeip-gateway-services.json")
        routes = read_json(self.demo / "signal_routes.json")["routes"]
        for route in routes:
            if route["service"] != 17152:
                continue
            for direction, prefix in (("tx", "vehicle"), ("rx", "remote")):
                type_name = "body_control_module_" + direction
                for config in configs:
                    service = next(
                        item for item in config["service_types"] if item["service_type_name"].lstrip("/") == type_name
                    )
                    event = next(item for item in service["events"] if item["event_name"] == route["eventName"])
                    assert event["event_id"] == route[prefix + "Event"]
                    assert event["eventgroup_ids"] == [event["event_id"]]
                    boolean_rx = direction == "rx" and route["datatype"] == "boolean"
                    assert event["serialization_config_type"] == (
                        "BooleanSerializerConfig" if boolean_rx else "NullSerializerConfig"
                    )
                    if not boolean_rx:
                        assert event["serialization_config"]["max_message_size"] == route["maxPayloadSize"]
                service = next(item for item in lola["serviceTypes"] if item["serviceTypeName"] == "/" + type_name)
                event = next(
                    item for item in service["bindings"][0]["events"] if item["eventName"] == route["eventName"]
                )
                assert event["eventId"] == 2115 + route[prefix + "Event"] - 33840
                instance = next(item for item in lola["serviceInstances"] if item["serviceTypeName"] == "/" + type_name)
                assert instance["instances"][0]["instanceId"] == route[prefix + "Instance"]
                assert route["eventName"] in {item["eventName"] for item in instance["instances"][0]["events"]}
                service = next(
                    item
                    for item in network["services"]
                    if int(item["service"], 0) == route["service"]
                    and int(item["instance"], 0) == route[prefix + "Instance"]
                )
                assert route[prefix + "Event"] in {int(item["event"], 0) for item in service["events"]}
                group = next(
                    item for item in service["eventgroups"] if int(item["eventgroup"], 0) == route[prefix + "Event"]
                )
                assert [int(event, 0) for event in group["events"]] == [route[prefix + "Event"]]

    def test_score_interfaces_and_config_match_gateway(self):
        example_root = ROOT / "communication/score/mw/com/example/vehicle-dynamics-example"
        declarations = example_root / "com-api-gen/body_signal_events.inc"
        assert declarations.read_text() == body_signal_declarations(ROOT)
        assert len(declarations.read_text().splitlines()) == 44
        example = read_json(example_root / "etc/mw_com_config.json")
        gateway = read_json(self.gateway / "score/gatewayd/etc/mw_com_config.json")
        for key in ("serviceTypes", "serviceInstances"):
            for type_name in ("/body_control_module_tx", "/body_control_module_rx"):
                expected = [item for item in gateway[key] if item["serviceTypeName"] == type_name]
                actual = [item for item in example[key] if item["serviceTypeName"] == type_name]
                assert actual == expected

    def test_snapshots_are_current_and_keep_existing_speed_route(self):
        for path, expected in configure(ROOT).items():
            assert read_json(path) == expected, str(path)
        routes = read_json(self.demo / "signal_routes.json")["routes"]
        speed = next(route for route in routes if route["name"] == "vehicle_dynamics")
        assert (speed["service"], speed["vehicleEvent"], speed["remoteEvent"]) == (6433, 1, 1)
        for name, event in (("high_beam", 33840), ("horn", 33844)):
            route = next(route for route in routes if route["name"] == name)
            assert route["vehicleEvent"] == event

    def test_low_beam_is_removed_from_generated_outputs(self):
        assert "low_beam_state" not in body_signal_declarations(ROOT)
        for path, document in configure(ROOT).items():
            assert "low_beam" not in str(document), str(path)
        routes = configure(ROOT)[self.demo / "signal_routes.json"]["routes"]
        assert len(routes) == 45
        assert not any(route["service"] == 17152 and route["vehicleEvent"] == 33842 for route in routes)


if __name__ == "__main__":
    unittest.main()
