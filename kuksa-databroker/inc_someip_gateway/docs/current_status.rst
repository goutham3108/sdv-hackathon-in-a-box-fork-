..
   # *******************************************************************************
   # Copyright (c) 2026 Contributors to the Eclipse Foundation
   #
   # See the NOTICE file(s) distributed with this work for additional
   # information regarding copyright ownership.
   #
   # This program and the accompanying materials are made available under the
   # terms of the Apache License Version 2.0 which is available at
   # https://www.apache.org/licenses/LICENSE-2.0
   #
   # SPDX-License-Identifier: Apache-2.0
   # *******************************************************************************

Current Project Status
======================

This document captures the current integration status of the SDV Hackathon-in-a-Box
setup shown in the architecture diagram. It focuses on the bring-up status on the
Raspberry Pi, the SDV runtime integration, and the remaining end-to-end validation
work.

Overview
--------

The current system is built around the following flow:

- input device or publisher side generates a signal or command
- SCORE components handle the middleware communication path
- ``someipd`` and ``gatewayd`` bridge the application side into SOME/IP
- KUKSA Databroker stores and serves VSS signals
- dashboard, CLI, or output device consumes the signal updates

At this stage, the platform bring-up is complete and the runtime integration on the
Raspberry Pi is in place. The remaining work is to complete the end-to-end testing
matrix and record the validation evidence for the current system state.

Current Status
--------------

The following items are currently true:

- SCORE runtime has been brought up on the Raspberry Pi
- SDV runtime integration has been connected on the Raspberry Pi
- SOME/IP gateway components are present in the current system architecture
- KUKSA Databroker is part of the end-to-end signal path
- CLI-based signal inspection is available for observing live updates
- the project now has a working baseline for end-to-end validation

The following items are still in progress or need final confirmation:

- full end-to-end testing of the complete signal path
- repeated validation of publisher to SOME/IP to Databroker to output flow
- confirmation of all configured signals in the current demo setup
- collection of pass/fail evidence for the current system status

Validated Integration Points
----------------------------

The current system should be validated across these integration points:

===========================  =========================================
Layer                        Validation goal
===========================  =========================================
Publisher / input device     Generate a signal or event at the source
SOME/IP gateway path         Confirm the signal reaches the gateway
SCORE runtime                Confirm runtime services stay healthy
KUKSA Databroker             Confirm the signal is written and visible
CLI / dashboard              Confirm the updated value is observable
Output device                Confirm the final action is triggered
===========================  =========================================

Suggested End-to-End Test Plan
------------------------------

1. Start the Raspberry Pi runtime stack.
2. Bring up the SCORE components and the SDV runtime services.
3. Start ``someipd`` and ``gatewayd``.
4. Start or connect the KUKSA Databroker.
5. Trigger a known signal from the publisher or input device.
6. Verify the signal reaches the Databroker and is visible on the CLI.
7. Verify the output device or dashboard reflects the updated state.
8. Repeat the same flow for each demo signal that is part of the current scope.

Known Demo Signals
------------------

The current demo scope includes at least the following validation candidates:

- tire pressure signal update
- headlight signal update

These signals are useful because they exercise both the data path and the observable
output path, making them suitable for a current-status validation report.

Open Items
----------

- complete the end-to-end test execution on the current hardware and software stack
- capture the exact commands used for the validation run
- record the final results for each signal flow
- note any remaining environment dependencies for repeatability

Acceptance Criteria for Current Status Sign-Off
-----------------------------------------------

The current project status can be considered validated when the following are true:

- the Raspberry Pi runtime stack starts without manual recovery steps
- the SCORE and SDV runtime integration remains stable during the test run
- the selected demo signals can be propagated end to end
- the Databroker CLI shows the expected live value updates
- the output side reflects the final signal state
- the observed results are recorded in this document or a linked test report

Next Step
---------

Run the full end-to-end validation flow and update this document with the final test
results, signal-by-signal observations, and any issues found during the run.
