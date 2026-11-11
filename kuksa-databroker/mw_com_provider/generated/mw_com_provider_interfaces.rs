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

pub mod vehicle {
    use score_com::{ProviderInfo, Publisher, Subscriber};

    score_com::interface!(interface BodyControlModuleService {
        Id = "BodyControlModuleService",
        horn_is_active: Event<crate::mw_com_provider_types::vehicle::HornIsActive>,
        low_beam_is_on: Event<crate::mw_com_provider_types::vehicle::LowBeamIsOn>,
        high_beam_is_on: Event<crate::mw_com_provider_types::vehicle::HighBeamIsOn>,
        high_beam_switch_is_on: Event<crate::mw_com_provider_types::vehicle::HighBeamSwitchIsOn>,
        parking_lights_are_on: Event<crate::mw_com_provider_types::vehicle::ParkingLightsAreOn>,
        running_lights_are_on: Event<crate::mw_com_provider_types::vehicle::RunningLightsAreOn>,
        front_fog_lights_are_on: Event<crate::mw_com_provider_types::vehicle::FrontFogLightsAreOn>,
        rear_fog_lights_are_on: Event<crate::mw_com_provider_types::vehicle::RearFogLightsAreOn>,
        hazard_lights_are_signaling: Event<crate::mw_com_provider_types::vehicle::HazardLightsAreSignaling>,
        left_indicator_is_signaling: Event<crate::mw_com_provider_types::vehicle::LeftIndicatorIsSignaling>,
        right_indicator_is_signaling: Event<crate::mw_com_provider_types::vehicle::RightIndicatorIsSignaling>,
        brake_lights_are_active: Event<crate::mw_com_provider_types::vehicle::BrakeLightsAreActive>,
        hood_is_open: Event<crate::mw_com_provider_types::vehicle::HoodIsOpen>,
        rear_trunk_is_open: Event<crate::mw_com_provider_types::vehicle::RearTrunkIsOpen>,
        rear_trunk_is_locked: Event<crate::mw_com_provider_types::vehicle::RearTrunkIsLocked>,
        rear_trunk_position: Event<crate::mw_com_provider_types::vehicle::RearTrunkPosition>,
        driver_mirror_is_folded: Event<crate::mw_com_provider_types::vehicle::DriverMirrorIsFolded>,
        passenger_mirror_is_folded: Event<crate::mw_com_provider_types::vehicle::PassengerMirrorIsFolded>,
        driver_mirror_heating_is_on: Event<crate::mw_com_provider_types::vehicle::DriverMirrorHeatingIsOn>,
        passenger_mirror_heating_is_on: Event<crate::mw_com_provider_types::vehicle::PassengerMirrorHeatingIsOn>,
        front_wiper_mode: Event<crate::mw_com_provider_types::vehicle::FrontWiperMode>,
        front_wiper_intensity: Event<crate::mw_com_provider_types::vehicle::FrontWiperIntensity>,
        front_washer_fluid_level: Event<crate::mw_com_provider_types::vehicle::FrontWasherFluidLevel>,
        front_washer_fluid_is_low: Event<crate::mw_com_provider_types::vehicle::FrontWasherFluidIsLow>,
        rain_intensity: Event<crate::mw_com_provider_types::vehicle::RainIntensity>,
        front_windshield_heating_is_on: Event<crate::mw_com_provider_types::vehicle::FrontWindshieldHeatingIsOn>,
        rear_windshield_heating_is_on: Event<crate::mw_com_provider_types::vehicle::RearWindshieldHeatingIsOn>,
        driver_door_is_locked: Event<crate::mw_com_provider_types::vehicle::DriverDoorIsLocked>,
        driver_door_is_open: Event<crate::mw_com_provider_types::vehicle::DriverDoorIsOpen>,
        driver_door_window_position: Event<crate::mw_com_provider_types::vehicle::DriverDoorWindowPosition>,
        driver_door_child_lock_is_active: Event<crate::mw_com_provider_types::vehicle::DriverDoorChildLockIsActive>,
        front_passenger_door_is_locked: Event<crate::mw_com_provider_types::vehicle::FrontPassengerDoorIsLocked>,
        front_passenger_door_is_open: Event<crate::mw_com_provider_types::vehicle::FrontPassengerDoorIsOpen>,
        front_passenger_door_window_position: Event<crate::mw_com_provider_types::vehicle::FrontPassengerDoorWindowPosition>,
        rear_driver_door_is_locked: Event<crate::mw_com_provider_types::vehicle::RearDriverDoorIsLocked>,
        rear_driver_door_is_open: Event<crate::mw_com_provider_types::vehicle::RearDriverDoorIsOpen>,
        rear_passenger_door_is_locked: Event<crate::mw_com_provider_types::vehicle::RearPassengerDoorIsLocked>,
        rear_passenger_door_is_open: Event<crate::mw_com_provider_types::vehicle::RearPassengerDoorIsOpen>,
        vehicle_speed: Event<crate::mw_com_provider_types::vehicle::VehicleSpeed>,
    });
}
