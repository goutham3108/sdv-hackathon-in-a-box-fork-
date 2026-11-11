/********************************************************************************
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

#ifndef SCORE_MW_COM_EXAMPLE_VEHICLE_DYNAMICS_EXAMPLE_VEHICLE_GEN_H
#define SCORE_MW_COM_EXAMPLE_VEHICLE_DYNAMICS_EXAMPLE_VEHICLE_GEN_H

#include <array>
#include <cstdint>
#include <cstddef>

#include "score/mw/com/types.h"

namespace score::mw::com
{

struct SpeedSample
{
    double value;
    std::uint8_t quality;
};

struct SpeedAck
{
    double value;
    std::uint8_t quality;
};

struct HighBeamState
{
    bool value;
};

struct HighBeamCommand
{
    std::size_t size{};
    alignas(std::max_align_t) std::uint8_t data[16]{};
};

static_assert(sizeof(HighBeamCommand) == 32U);
static_assert(alignof(HighBeamCommand) == 16U);
static_assert(offsetof(HighBeamCommand, data) == 16U);

struct BodySignalText
{
    std::size_t size{};
    alignas(std::max_align_t) std::uint8_t data[32]{};
};

struct BodySignalPayload
{
    std::size_t size{};
    alignas(std::max_align_t) std::uint8_t data[32]{};
};

static_assert(sizeof(BodySignalPayload) == 48U);
static_assert(alignof(BodySignalPayload) == 16U);
static_assert(offsetof(BodySignalPayload, data) == 16U);

static_assert(sizeof(BodySignalText) == 48U);
static_assert(alignof(BodySignalText) == 16U);
static_assert(offsetof(BodySignalText, data) == 16U);
static_assert(sizeof(HighBeamState) == sizeof(bool));

#define SCORE_BODY_BOOL_SAMPLE(name) \
    struct name                         \
    {                                   \
        bool value;                     \
    };

SCORE_BODY_BOOL_SAMPLE(HornIsActive)
SCORE_BODY_BOOL_SAMPLE(LowBeamIsOn)
SCORE_BODY_BOOL_SAMPLE(HighBeamIsOn)
SCORE_BODY_BOOL_SAMPLE(HighBeamSwitchIsOn)
SCORE_BODY_BOOL_SAMPLE(ParkingLightsAreOn)
SCORE_BODY_BOOL_SAMPLE(RunningLightsAreOn)
SCORE_BODY_BOOL_SAMPLE(FrontFogLightsAreOn)
SCORE_BODY_BOOL_SAMPLE(RearFogLightsAreOn)
SCORE_BODY_BOOL_SAMPLE(HazardLightsAreSignaling)
SCORE_BODY_BOOL_SAMPLE(LeftIndicatorIsSignaling)
SCORE_BODY_BOOL_SAMPLE(RightIndicatorIsSignaling)
SCORE_BODY_BOOL_SAMPLE(BrakeLightsAreActive)
SCORE_BODY_BOOL_SAMPLE(HoodIsOpen)
SCORE_BODY_BOOL_SAMPLE(RearTrunkIsOpen)
SCORE_BODY_BOOL_SAMPLE(RearTrunkIsLocked)
SCORE_BODY_BOOL_SAMPLE(DriverMirrorIsFolded)
SCORE_BODY_BOOL_SAMPLE(PassengerMirrorIsFolded)
SCORE_BODY_BOOL_SAMPLE(DriverMirrorHeatingIsOn)
SCORE_BODY_BOOL_SAMPLE(PassengerMirrorHeatingIsOn)
SCORE_BODY_BOOL_SAMPLE(FrontWasherFluidIsLow)
SCORE_BODY_BOOL_SAMPLE(FrontWindshieldHeatingIsOn)
SCORE_BODY_BOOL_SAMPLE(RearWindshieldHeatingIsOn)
SCORE_BODY_BOOL_SAMPLE(DriverDoorIsLocked)
SCORE_BODY_BOOL_SAMPLE(DriverDoorIsOpen)
SCORE_BODY_BOOL_SAMPLE(DriverDoorChildLockIsActive)
SCORE_BODY_BOOL_SAMPLE(FrontPassengerDoorIsLocked)
SCORE_BODY_BOOL_SAMPLE(FrontPassengerDoorIsOpen)
SCORE_BODY_BOOL_SAMPLE(RearDriverDoorIsLocked)
SCORE_BODY_BOOL_SAMPLE(RearDriverDoorIsOpen)
SCORE_BODY_BOOL_SAMPLE(RearPassengerDoorIsLocked)
SCORE_BODY_BOOL_SAMPLE(RearPassengerDoorIsOpen)

#undef SCORE_BODY_BOOL_SAMPLE

#define SCORE_BODY_U8_SAMPLE(name) \
    struct name                      \
    {                                \
        std::uint8_t value;          \
    };

SCORE_BODY_U8_SAMPLE(RearTrunkPosition)
SCORE_BODY_U8_SAMPLE(FrontWiperIntensity)
SCORE_BODY_U8_SAMPLE(FrontWasherFluidLevel)
SCORE_BODY_U8_SAMPLE(RainIntensity)
SCORE_BODY_U8_SAMPLE(DriverDoorWindowPosition)
SCORE_BODY_U8_SAMPLE(FrontPassengerDoorWindowPosition)

#undef SCORE_BODY_U8_SAMPLE

struct FrontWiperMode
{
    std::array<std::uint8_t, 16> value{};
};

struct VehicleSpeed
{
    float value;
};

static_assert(sizeof(FrontWiperMode) == 16U);
static_assert(sizeof(VehicleSpeed) == sizeof(float));

template <typename Trait>
class VehicleDynamicsService : public Trait::Base
{
  public:
    using Trait::Base::Base;

    typename Trait::template Event<SpeedSample> speed{*this, "speed"};

    typename Trait::template Event<SpeedAck> speedAck{*this, "speedAck"};
};

using VehicleDynamicsServiceProxy = AsProxy<VehicleDynamicsService>;
using VehicleDynamicsServiceSkeleton = AsSkeleton<VehicleDynamicsService>;

template <typename Trait>
class VehicleHighBeamRx : public Trait::Base
{
    public:
        using Trait::Base::Base;

    #define SCORE_BODY_SIGNAL_EVENT(rx_type, tx_type, event_name) \
        typename Trait::template Event<rx_type> event_name{*this, #event_name};
    #include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/body_signal_events.inc"
    #undef SCORE_BODY_SIGNAL_EVENT
};

using VehicleHighBeamRxProxy = AsProxy<VehicleHighBeamRx>;
using VehicleHighBeamRxSkeleton = AsSkeleton<VehicleHighBeamRx>;

template <typename Trait>
class HighBeamTx : public Trait::Base
{
    public:
        using Trait::Base::Base;
    #define SCORE_BODY_SIGNAL_EVENT(rx_type, tx_type, event_name) \
        typename Trait::template Event<tx_type> event_name{*this, #event_name};
    #include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/body_signal_events.inc"
    #undef SCORE_BODY_SIGNAL_EVENT
};

using HighBeamTxProxy = AsProxy<HighBeamTx>;
using HighBeamTxSkeleton = AsSkeleton<HighBeamTx>;

template <typename Trait>
class BodyControlModuleService : public Trait::Base
{
    public:
        using Trait::Base::Base;

#define SCORE_BODY_EVENT(type, name) typename Trait::template Event<type> name{*this, #name};
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
};

using BodyControlModuleServiceProxy = AsProxy<BodyControlModuleService>;
using BodyControlModuleServiceSkeleton = AsSkeleton<BodyControlModuleService>;

}  // namespace score::mw::com

#endif  // SCORE_MW_COM_EXAMPLE_VEHICLE_DYNAMICS_EXAMPLE_VEHICLE_GEN_H