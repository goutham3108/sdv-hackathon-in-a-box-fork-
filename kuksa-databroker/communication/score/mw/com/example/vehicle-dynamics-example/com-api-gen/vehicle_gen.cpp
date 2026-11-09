/*********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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

#include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/vehicle_gen.h"

#include "score/mw/com/rust/score_com_cpp_bridge/register_interface.h"

BEGIN_EXPORT_MW_COM_INTERFACE(
    VehicleDynamicsService,
    ::score::mw::com::VehicleDynamicsServiceProxy,
    ::score::mw::com::VehicleDynamicsServiceSkeleton)

EXPORT_MW_COM_EVENT(::score::mw::com::SpeedSample, speed)
EXPORT_MW_COM_EVENT(::score::mw::com::SpeedAck, speedAck)

END_EXPORT_MW_COM_INTERFACE()

EXPORT_MW_COM_TYPE(SpeedSample, ::score::mw::com::SpeedSample)
EXPORT_MW_COM_TYPE(SpeedAck, ::score::mw::com::SpeedAck)

BEGIN_EXPORT_MW_COM_INTERFACE(
    VehicleHighBeamRx,
    ::score::mw::com::VehicleHighBeamRxProxy,
    ::score::mw::com::VehicleHighBeamRxSkeleton)

#define SCORE_BODY_SIGNAL_EVENT(rx_type, tx_type, event_name) \
EXPORT_MW_COM_EVENT(::score::mw::com::rx_type, event_name)
#include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/body_signal_events.inc"
#undef SCORE_BODY_SIGNAL_EVENT

END_EXPORT_MW_COM_INTERFACE()

EXPORT_MW_COM_TYPE(HighBeamState, ::score::mw::com::HighBeamState)
EXPORT_MW_COM_TYPE(BodySignalText, ::score::mw::com::BodySignalText)
EXPORT_MW_COM_TYPE(BodySignalPayload, ::score::mw::com::BodySignalPayload)

BEGIN_EXPORT_MW_COM_INTERFACE(
    HighBeamTx,
    ::score::mw::com::HighBeamTxProxy,
    ::score::mw::com::HighBeamTxSkeleton)

#define SCORE_BODY_SIGNAL_EVENT(rx_type, tx_type, event_name) \
EXPORT_MW_COM_EVENT(::score::mw::com::tx_type, event_name)
#include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/body_signal_events.inc"
#undef SCORE_BODY_SIGNAL_EVENT

END_EXPORT_MW_COM_INTERFACE()

EXPORT_MW_COM_TYPE(HighBeamCommand, ::score::mw::com::HighBeamCommand)

BEGIN_EXPORT_MW_COM_INTERFACE(
    BodyControlModuleService,
    ::score::mw::com::BodyControlModuleServiceProxy,
    ::score::mw::com::BodyControlModuleServiceSkeleton)

#define SCORE_BODY_EVENT(type, name) EXPORT_MW_COM_EVENT(::score::mw::com::type, name)
SCORE_BODY_EVENT(HornIsActive, horn_is_active)
SCORE_BODY_EVENT(LowBeamIsOn, low_beam_is_on)
SCORE_BODY_EVENT(HighBeamIsOn, high_beam_is_on)
SCORE_BODY_EVENT(HighBeamSwitchIsOn, high_beam_switch_is_on)
SCORE_BODY_EVENT(ParkingLightsAreOn, parking_lights_are_on)
SCORE_BODY_EVENT(RunningLightsAreOn, running_lights_are_on)
SCORE_BODY_EVENT(FrontFogLightsAreOn, front_fog_lights_are_on)
SCORE_BODY_EVENT(RearFogLightsAreOn, rear_fog_lights_are_on)
SCORE_BODY_EVENT(HazardLightsAreSignaling, hazard_lights_are_signaling)
SCORE_BODY_EVENT(LeftIndicatorIsSignaling, left_indicator_is_signaling)
SCORE_BODY_EVENT(RightIndicatorIsSignaling, right_indicator_is_signaling)
SCORE_BODY_EVENT(BrakeLightsAreActive, brake_lights_are_active)
SCORE_BODY_EVENT(HoodIsOpen, hood_is_open)
SCORE_BODY_EVENT(RearTrunkIsOpen, rear_trunk_is_open)
SCORE_BODY_EVENT(RearTrunkIsLocked, rear_trunk_is_locked)
SCORE_BODY_EVENT(RearTrunkPosition, rear_trunk_position)
SCORE_BODY_EVENT(DriverMirrorIsFolded, driver_mirror_is_folded)
SCORE_BODY_EVENT(PassengerMirrorIsFolded, passenger_mirror_is_folded)
SCORE_BODY_EVENT(DriverMirrorHeatingIsOn, driver_mirror_heating_is_on)
SCORE_BODY_EVENT(PassengerMirrorHeatingIsOn, passenger_mirror_heating_is_on)
SCORE_BODY_EVENT(FrontWiperMode, front_wiper_mode)
SCORE_BODY_EVENT(FrontWiperIntensity, front_wiper_intensity)
SCORE_BODY_EVENT(FrontWasherFluidLevel, front_washer_fluid_level)
SCORE_BODY_EVENT(FrontWasherFluidIsLow, front_washer_fluid_is_low)
SCORE_BODY_EVENT(RainIntensity, rain_intensity)
SCORE_BODY_EVENT(FrontWindshieldHeatingIsOn, front_windshield_heating_is_on)
SCORE_BODY_EVENT(RearWindshieldHeatingIsOn, rear_windshield_heating_is_on)
SCORE_BODY_EVENT(DriverDoorIsLocked, driver_door_is_locked)
SCORE_BODY_EVENT(DriverDoorIsOpen, driver_door_is_open)
SCORE_BODY_EVENT(DriverDoorWindowPosition, driver_door_window_position)
SCORE_BODY_EVENT(DriverDoorChildLockIsActive, driver_door_child_lock_is_active)
SCORE_BODY_EVENT(FrontPassengerDoorIsLocked, front_passenger_door_is_locked)
SCORE_BODY_EVENT(FrontPassengerDoorIsOpen, front_passenger_door_is_open)
SCORE_BODY_EVENT(FrontPassengerDoorWindowPosition, front_passenger_door_window_position)
SCORE_BODY_EVENT(RearDriverDoorIsLocked, rear_driver_door_is_locked)
SCORE_BODY_EVENT(RearDriverDoorIsOpen, rear_driver_door_is_open)
SCORE_BODY_EVENT(RearPassengerDoorIsLocked, rear_passenger_door_is_locked)
SCORE_BODY_EVENT(RearPassengerDoorIsOpen, rear_passenger_door_is_open)
SCORE_BODY_EVENT(VehicleSpeed, vehicle_speed)
#undef SCORE_BODY_EVENT

END_EXPORT_MW_COM_INTERFACE()

#define SCORE_BODY_TYPE(type) EXPORT_MW_COM_TYPE(type, ::score::mw::com::type)
SCORE_BODY_TYPE(HornIsActive)
SCORE_BODY_TYPE(LowBeamIsOn)
SCORE_BODY_TYPE(HighBeamIsOn)
SCORE_BODY_TYPE(HighBeamSwitchIsOn)
SCORE_BODY_TYPE(ParkingLightsAreOn)
SCORE_BODY_TYPE(RunningLightsAreOn)
SCORE_BODY_TYPE(FrontFogLightsAreOn)
SCORE_BODY_TYPE(RearFogLightsAreOn)
SCORE_BODY_TYPE(HazardLightsAreSignaling)
SCORE_BODY_TYPE(LeftIndicatorIsSignaling)
SCORE_BODY_TYPE(RightIndicatorIsSignaling)
SCORE_BODY_TYPE(BrakeLightsAreActive)
SCORE_BODY_TYPE(HoodIsOpen)
SCORE_BODY_TYPE(RearTrunkIsOpen)
SCORE_BODY_TYPE(RearTrunkIsLocked)
SCORE_BODY_TYPE(RearTrunkPosition)
SCORE_BODY_TYPE(DriverMirrorIsFolded)
SCORE_BODY_TYPE(PassengerMirrorIsFolded)
SCORE_BODY_TYPE(DriverMirrorHeatingIsOn)
SCORE_BODY_TYPE(PassengerMirrorHeatingIsOn)
SCORE_BODY_TYPE(FrontWiperMode)
SCORE_BODY_TYPE(FrontWiperIntensity)
SCORE_BODY_TYPE(FrontWasherFluidLevel)
SCORE_BODY_TYPE(FrontWasherFluidIsLow)
SCORE_BODY_TYPE(RainIntensity)
SCORE_BODY_TYPE(FrontWindshieldHeatingIsOn)
SCORE_BODY_TYPE(RearWindshieldHeatingIsOn)
SCORE_BODY_TYPE(DriverDoorIsLocked)
SCORE_BODY_TYPE(DriverDoorIsOpen)
SCORE_BODY_TYPE(DriverDoorWindowPosition)
SCORE_BODY_TYPE(DriverDoorChildLockIsActive)
SCORE_BODY_TYPE(FrontPassengerDoorIsLocked)
SCORE_BODY_TYPE(FrontPassengerDoorIsOpen)
SCORE_BODY_TYPE(FrontPassengerDoorWindowPosition)
SCORE_BODY_TYPE(RearDriverDoorIsLocked)
SCORE_BODY_TYPE(RearDriverDoorIsOpen)
SCORE_BODY_TYPE(RearPassengerDoorIsLocked)
SCORE_BODY_TYPE(RearPassengerDoorIsOpen)
SCORE_BODY_TYPE(VehicleSpeed)
#undef SCORE_BODY_TYPE