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

# Startup Hook

The crate exposes `MwComProvider::from_generated_config`,
`MwComProvider::from_config_file`, and `MwComProvider::register_with_broker` as
the startup integration points. Use `from_generated_config` when starting from
the JSON produced by the external code generator.

A future Databroker feature can mirror the IEEE1722 startup pattern:

1. Add a feature such as `mw-com-provider` to the Databroker package.
2. Add CLI arguments for the generated config path and mw::com runtime parameters.
3. Construct the concrete mw::com transport.
4. Call `MwComProvider::from_generated_config` or `MwComProvider::from_config_file`.
5. Call `register_with_broker()` and spawn `run_until_shutdown` alongside the existing gRPC/VISS tasks.

The provider and mapper implement both the inbound datapoint path and outbound
actuation path. Real SCORE/native middleware libraries, application startup
wiring, and end-to-end validation remain integration work.
