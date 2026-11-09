/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

use score_com::{
    interface,
    CommData,
    ProviderInfo,
    Publisher,
    Reloc,
    Subscriber,
};

use score_log::ScoreDebug;

#[derive(Debug, Reloc, CommData, ScoreDebug)]
#[repr(C)]
#[comm_data(id = "SpeedSample")]
pub struct SpeedSample {
    pub value: f64,
    pub quality: u8,
}

#[derive(Debug, Reloc, CommData, ScoreDebug)]
#[repr(C)]
#[comm_data(id = "SpeedAck")]
pub struct SpeedAck {
    pub value: f64,
    pub quality: u8,
}

interface!(
    interface VehicleDynamicsService {
        Id = "VehicleDynamicsService",

        // Producer -> Consumer
        speed: Event<SpeedSample>,

        // mw_com_provider -> SCORE Consumer
        speedAck: Event<SpeedAck>,
    }
);

macro_rules! body_sample {
    ($name:ident, $value_type:ty, $type_id:literal) => {
        #[derive(Debug, Reloc, CommData, ScoreDebug)]
        #[repr(C)]
        #[comm_data(id = $type_id)]
        pub struct $name {
            pub value: $value_type,
        }
    };
}

body_sample!(HornIsActive, bool, "HornIsActive");
body_sample!(LowBeamIsOn, bool, "LowBeamIsOn");
body_sample!(HighBeamIsOn, bool, "HighBeamIsOn");
body_sample!(HighBeamSwitchIsOn, bool, "HighBeamSwitchIsOn");
body_sample!(ParkingLightsAreOn, bool, "ParkingLightsAreOn");
body_sample!(RunningLightsAreOn, bool, "RunningLightsAreOn");
body_sample!(FrontFogLightsAreOn, bool, "FrontFogLightsAreOn");
body_sample!(RearFogLightsAreOn, bool, "RearFogLightsAreOn");
body_sample!(HazardLightsAreSignaling, bool, "HazardLightsAreSignaling");
body_sample!(LeftIndicatorIsSignaling, bool, "LeftIndicatorIsSignaling");
body_sample!(RightIndicatorIsSignaling, bool, "RightIndicatorIsSignaling");
body_sample!(BrakeLightsAreActive, bool, "BrakeLightsAreActive");
body_sample!(HoodIsOpen, bool, "HoodIsOpen");
body_sample!(RearTrunkIsOpen, bool, "RearTrunkIsOpen");
body_sample!(RearTrunkIsLocked, bool, "RearTrunkIsLocked");
body_sample!(RearTrunkPosition, u8, "RearTrunkPosition");
body_sample!(DriverMirrorIsFolded, bool, "DriverMirrorIsFolded");
body_sample!(PassengerMirrorIsFolded, bool, "PassengerMirrorIsFolded");
body_sample!(DriverMirrorHeatingIsOn, bool, "DriverMirrorHeatingIsOn");
body_sample!(PassengerMirrorHeatingIsOn, bool, "PassengerMirrorHeatingIsOn");
body_sample!(FrontWiperMode, [u8; 16], "FrontWiperMode");
body_sample!(FrontWiperIntensity, u8, "FrontWiperIntensity");
body_sample!(FrontWasherFluidLevel, u8, "FrontWasherFluidLevel");
body_sample!(FrontWasherFluidIsLow, bool, "FrontWasherFluidIsLow");
body_sample!(RainIntensity, u8, "RainIntensity");
body_sample!(FrontWindshieldHeatingIsOn, bool, "FrontWindshieldHeatingIsOn");
body_sample!(RearWindshieldHeatingIsOn, bool, "RearWindshieldHeatingIsOn");
body_sample!(DriverDoorIsLocked, bool, "DriverDoorIsLocked");
body_sample!(DriverDoorIsOpen, bool, "DriverDoorIsOpen");
body_sample!(DriverDoorWindowPosition, u8, "DriverDoorWindowPosition");
body_sample!(DriverDoorChildLockIsActive, bool, "DriverDoorChildLockIsActive");
body_sample!(FrontPassengerDoorIsLocked, bool, "FrontPassengerDoorIsLocked");
body_sample!(FrontPassengerDoorIsOpen, bool, "FrontPassengerDoorIsOpen");
body_sample!(FrontPassengerDoorWindowPosition, u8, "FrontPassengerDoorWindowPosition");
body_sample!(RearDriverDoorIsLocked, bool, "RearDriverDoorIsLocked");
body_sample!(RearDriverDoorIsOpen, bool, "RearDriverDoorIsOpen");
body_sample!(RearPassengerDoorIsLocked, bool, "RearPassengerDoorIsLocked");
body_sample!(RearPassengerDoorIsOpen, bool, "RearPassengerDoorIsOpen");
body_sample!(VehicleSpeed, f32, "VehicleSpeed");

interface!(
    interface BodyControlModuleService {
        Id = "BodyControlModuleService",
        horn_is_active: Event<HornIsActive>,
        low_beam_is_on: Event<LowBeamIsOn>,
        high_beam_is_on: Event<HighBeamIsOn>,
        high_beam_switch_is_on: Event<HighBeamSwitchIsOn>,
        parking_lights_are_on: Event<ParkingLightsAreOn>,
        running_lights_are_on: Event<RunningLightsAreOn>,
        front_fog_lights_are_on: Event<FrontFogLightsAreOn>,
        rear_fog_lights_are_on: Event<RearFogLightsAreOn>,
        hazard_lights_are_signaling: Event<HazardLightsAreSignaling>,
        left_indicator_is_signaling: Event<LeftIndicatorIsSignaling>,
        right_indicator_is_signaling: Event<RightIndicatorIsSignaling>,
        brake_lights_are_active: Event<BrakeLightsAreActive>,
        hood_is_open: Event<HoodIsOpen>,
        rear_trunk_is_open: Event<RearTrunkIsOpen>,
        rear_trunk_is_locked: Event<RearTrunkIsLocked>,
        rear_trunk_position: Event<RearTrunkPosition>,
        driver_mirror_is_folded: Event<DriverMirrorIsFolded>,
        passenger_mirror_is_folded: Event<PassengerMirrorIsFolded>,
        driver_mirror_heating_is_on: Event<DriverMirrorHeatingIsOn>,
        passenger_mirror_heating_is_on: Event<PassengerMirrorHeatingIsOn>,
        front_wiper_mode: Event<FrontWiperMode>,
        front_wiper_intensity: Event<FrontWiperIntensity>,
        front_washer_fluid_level: Event<FrontWasherFluidLevel>,
        front_washer_fluid_is_low: Event<FrontWasherFluidIsLow>,
        rain_intensity: Event<RainIntensity>,
        front_windshield_heating_is_on: Event<FrontWindshieldHeatingIsOn>,
        rear_windshield_heating_is_on: Event<RearWindshieldHeatingIsOn>,
        driver_door_is_locked: Event<DriverDoorIsLocked>,
        driver_door_is_open: Event<DriverDoorIsOpen>,
        driver_door_window_position: Event<DriverDoorWindowPosition>,
        driver_door_child_lock_is_active: Event<DriverDoorChildLockIsActive>,
        front_passenger_door_is_locked: Event<FrontPassengerDoorIsLocked>,
        front_passenger_door_is_open: Event<FrontPassengerDoorIsOpen>,
        front_passenger_door_window_position: Event<FrontPassengerDoorWindowPosition>,
        rear_driver_door_is_locked: Event<RearDriverDoorIsLocked>,
        rear_driver_door_is_open: Event<RearDriverDoorIsOpen>,
        rear_passenger_door_is_locked: Event<RearPassengerDoorIsLocked>,
        rear_passenger_door_is_open: Event<RearPassengerDoorIsOpen>,
        vehicle_speed: Event<VehicleSpeed>,
    }
);