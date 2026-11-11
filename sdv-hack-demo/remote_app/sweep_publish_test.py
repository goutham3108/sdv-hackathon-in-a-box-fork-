# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

"""Check high-beam sweep timing using loopback UDP and no GPIO hardware."""

import argparse
import json
import os
from pathlib import Path
import socket
import subprocess
import tempfile
import time


def main(binary):
    source = Path(__file__).resolve().parents[1] / "signal_routes.json"
    route = next(
        route for route in json.loads(source.read_text())["routes"]
        if route["name"] == "high_beam"
    )
    with tempfile.TemporaryDirectory(prefix="sdv-sweep-test-") as directory:
        root = Path(directory)
        config = root / "routes.json"
        config.write_text(json.dumps({"routes": [route]}))
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as receiver:
            receiver.bind(("127.0.0.1", 0))
            receiver.settimeout(0.25)
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as reservation:
                reservation.bind(("127.0.0.1", 0))
                remote_port = reservation.getsockname()[1]
            env = dict(os.environ)
            env.update({
                "SIGNAL_ROUTE_CONFIG": str(config),
                "SDV_BRIDGE_IP": "127.0.0.1",
                "SDV_BIND_IP": "127.0.0.1",
                "SDV_BRIDGE_UDP_PORT": str(receiver.getsockname()[1]),
                "SDV_REMOTE_UDP_PORT": str(remote_port),
                "HIGH_BEAM_GPIO_CHIP": str(root / "no-gpio"),
                "HIGH_BEAM_GPIO_OUTPUT_CHIP": str(root / "no-gpio"),
                "SDV_KVS_DIR": str(root / "state"),
            })
            with (root / "remote.log").open("w+") as output:
                process = subprocess.Popen(
                    [str(Path(binary).resolve())],
                    stdin=subprocess.PIPE,
                    stdout=output,
                    stderr=subprocess.STDOUT,
                    env=env,
                    start_new_session=True,
                    text=True,
                )
                samples = []
                try:
                    process.stdin.write("4\n")
                    process.stdin.flush()
                    deadline = time.monotonic() + 25
                    quit_at = time.monotonic() + 17
                    quit_sent = False
                    while time.monotonic() < deadline:
                        if not quit_sent and time.monotonic() >= quit_at:
                            process.stdin.write("q\n")
                            process.stdin.flush()
                            quit_sent = True
                        try:
                            data, _ = receiver.recvfrom(1024)
                        except socket.timeout:
                            if process.poll() is not None:
                                break
                            continue
                        if len(data) >= 15 and int.from_bytes(data[10:12], "big") == route["remoteEvent"]:
                            samples.append((time.monotonic(), data[14]))
                    process.wait(timeout=3)
                    if process.returncode:
                        output.seek(0)
                        raise AssertionError(output.read())
                    first_true = next(t for t, value in samples if value == 1)
                    following_false = next(t for t, value in samples if value == 0 and t > first_true)
                    assert following_false - first_true >= 4.5, samples
                    assert any(value == 0 and t < first_true for t, value in samples), samples
                    print("PASS: false -> true -> false; periodic publisher did not overwrite test true")
                finally:
                    if process.poll() is None:
                        process.terminate()
                        process.wait(timeout=5)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("binary")
    main(parser.parse_args().binary)
