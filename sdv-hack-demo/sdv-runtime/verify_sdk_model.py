# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

"""Verify generated SDK reads in a disposable runtime container."""

import asyncio
import json
import os
import socket
import subprocess
import time
from types import SimpleNamespace


async def verify():
    from vehicle import vehicle
    from velocitas_sdk.model import BrokerDatapoint
    from velocitas_sdk.vdb.client import VehicleDataBrokerClient

    cases = {
        "Vehicle.Speed": ("float_value", 113.0),
        "Vehicle.Body.Lights.Brake.IsActive": ("string_value", "ACTIVE"),
        "Vehicle.OBD.OilTemperature": ("float_value", 85.0),
        "Vehicle.Body.Trunk.Rear.Position": ("uint32_value", 50),
        "Vehicle.Body.Mirrors.DriverSide.IsFolded": ("bool_value", True),
        "Vehicle.Body.Mirrors.PassengerSide.IsFolded": ("bool_value", True),
        "Vehicle.Powertrain.TractionBattery.CellVoltage.CellVoltages": (
            "float_array",
            [3.5, 3.75],
        ),
    }
    fixtures = {
        "bool": ("bool_value", True),
        "uint8": ("uint32_value", 50),
        "float": ("float_value", 85.0),
        "float_array": ("float_array", [3.5, 3.75]),
        "string": ("string_value", "OFF"),
    }
    for path, datatype in json.loads(os.environ.get("SDK_SIGNAL_TYPES", "{}")).items():
        if path != "Vehicle.Powertrain.Transmission.PerformanceMode":
            cases.setdefault(path, fixtures[datatype])
    points = {}
    for path, (field, value) in cases.items():
        point = BrokerDatapoint()
        if field.endswith("_array"):
            getattr(point, field).values.extend(value)
        else:
            setattr(point, field, value)
        points[path] = point

    client = VehicleDataBrokerClient()
    original_get = client.GetDatapoints

    async def get_datapoints(paths):
        return SimpleNamespace(datapoints={path: points[path] for path in paths})

    client.GetDatapoints = get_datapoints
    try:
        for path, (_, expected) in cases.items():
            node = vehicle
            for name in path.split(".")[1:]:
                node = getattr(node, name)
            actual = (await node.get()).value
            if isinstance(expected, list):
                actual = list(actual)
            assert actual == expected, (path, actual, expected)
            print(f"PASS {path}: {actual!r}", flush=True)
        print(f"PASS: {len(cases)} SDK properties decoded correctly", flush=True)
    finally:
        client.GetDatapoints = original_get
        await client.close()


def main():
    broker = subprocess.Popen(
        ["mosquitto", "-p", "1883"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
    )
    try:
        deadline = time.monotonic() + 10
        while True:
            if broker.poll() is not None:
                raise RuntimeError(f"Test MQTT broker exited: {broker.stderr.read()}")
            try:
                with socket.create_connection(("127.0.0.1", 1883), timeout=1):
                    break
            except OSError:
                if time.monotonic() >= deadline:
                    raise TimeoutError("Test MQTT broker did not become ready")
                time.sleep(0.1)
        asyncio.run(verify())
    finally:
        if broker.poll() is None:
            broker.terminate()
        broker.communicate(timeout=10)


if __name__ == "__main__":
    main()
