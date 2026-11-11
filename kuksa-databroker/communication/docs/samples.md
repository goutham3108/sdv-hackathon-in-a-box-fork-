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

# Sample Models

## Service Definition Model Input

The external code generator consumes the service definition model together with
the templates under `templates/reference-codegen/`.

```json
{
  "services": [
    {
      "name": "VehicleSpeedService",
      "instances": [
        {
          "name": "front_vehicle",
          "members": [
            {
              "name": "speed",
              "datatype": "double",
              "vss": {
                "signal_id": 42,
                "path": "Vehicle.Speed",
                "datatype": "double",
                "unit": "km/h",
                "scale": 1.0,
                "offset": 0.0,
                "min": 0.0,
                "max": 300.0,
                "direction": "datapoint",
                "cycle_time_ms": 100
              }
            }
          ]
        }
      ]
    }
  ]
}
```

## Generated Mapping JSON

```json
{
  "provider_name": "mw_com_provider",
  "queue_capacity": 256,
  "stale_after_ms": 1000,
  "mappings": [
    {
      "signal_id": 42,
      "vss_path": "Vehicle.Speed",
      "datatype": "double",
      "service": "VehicleSpeedService",
      "instance": "front_vehicle",
      "member": "speed",
      "unit": "km/h",
      "scale": 1.0,
      "offset": 0.0,
      "min": 0.0,
      "max": 300.0,
      "required_quality": "valid",
      "direction": "datapoint",
      "cycle_time_ms": 100
    }
  ]
}
```
