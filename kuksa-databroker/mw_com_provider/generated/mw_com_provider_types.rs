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

use score_com::{CommData, Reloc};
use std::default::Default;

pub mod vehicle {
    use super::*;
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "HornIsActive")]
    pub struct HornIsActive {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "LowBeamIsOn")]
    pub struct LowBeamIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "HighBeamIsOn")]
    pub struct HighBeamIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "HighBeamSwitchIsOn")]
    pub struct HighBeamSwitchIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "ParkingLightsAreOn")]
    pub struct ParkingLightsAreOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RunningLightsAreOn")]
    pub struct RunningLightsAreOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontFogLightsAreOn")]
    pub struct FrontFogLightsAreOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearFogLightsAreOn")]
    pub struct RearFogLightsAreOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "HazardLightsAreSignaling")]
    pub struct HazardLightsAreSignaling {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "LeftIndicatorIsSignaling")]
    pub struct LeftIndicatorIsSignaling {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RightIndicatorIsSignaling")]
    pub struct RightIndicatorIsSignaling {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "BrakeLightsAreActive")]
    pub struct BrakeLightsAreActive {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "HoodIsOpen")]
    pub struct HoodIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearTrunkIsOpen")]
    pub struct RearTrunkIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearTrunkIsLocked")]
    pub struct RearTrunkIsLocked {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearTrunkPosition")]
    pub struct RearTrunkPosition {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverMirrorIsFolded")]
    pub struct DriverMirrorIsFolded {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "PassengerMirrorIsFolded")]
    pub struct PassengerMirrorIsFolded {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverMirrorHeatingIsOn")]
    pub struct DriverMirrorHeatingIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "PassengerMirrorHeatingIsOn")]
    pub struct PassengerMirrorHeatingIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontWiperMode")]
    pub struct FrontWiperMode {
        pub value: [u8; 16],
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontWiperIntensity")]
    pub struct FrontWiperIntensity {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontWasherFluidLevel")]
    pub struct FrontWasherFluidLevel {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontWasherFluidIsLow")]
    pub struct FrontWasherFluidIsLow {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RainIntensity")]
    pub struct RainIntensity {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontWindshieldHeatingIsOn")]
    pub struct FrontWindshieldHeatingIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearWindshieldHeatingIsOn")]
    pub struct RearWindshieldHeatingIsOn {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverDoorIsLocked")]
    pub struct DriverDoorIsLocked {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverDoorIsOpen")]
    pub struct DriverDoorIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverDoorWindowPosition")]
    pub struct DriverDoorWindowPosition {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "DriverDoorChildLockIsActive")]
    pub struct DriverDoorChildLockIsActive {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontPassengerDoorIsLocked")]
    pub struct FrontPassengerDoorIsLocked {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontPassengerDoorIsOpen")]
    pub struct FrontPassengerDoorIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "FrontPassengerDoorWindowPosition")]
    pub struct FrontPassengerDoorWindowPosition {
        pub value: u8,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearDriverDoorIsLocked")]
    pub struct RearDriverDoorIsLocked {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearDriverDoorIsOpen")]
    pub struct RearDriverDoorIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearPassengerDoorIsLocked")]
    pub struct RearPassengerDoorIsLocked {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "RearPassengerDoorIsOpen")]
    pub struct RearPassengerDoorIsOpen {
        pub value: bool,
    }
    #[repr(C)]
    #[derive(Debug, Clone, PartialEq, Default, Reloc, CommData)]
    #[comm_data(id = "VehicleSpeed")]
    pub struct VehicleSpeed {
        pub value: f32,
    }
}
