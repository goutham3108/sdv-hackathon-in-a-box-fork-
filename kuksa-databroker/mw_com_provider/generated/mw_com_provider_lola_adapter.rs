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

use crate::error::MwComProviderError;
use crate::quality::SignalQuality;
use crate::score_bindings::{ScoreBinding, ScoreRuntimeAdapter};
use crate::transport::{MwComMessage, MwComValue};
use async_trait::async_trait;
use score_com::{
    Builder, FindServiceSpecifier, InstanceSpecifier, Interface, LolaRuntimeBuilderImpl,
    LolaRuntimeImpl, OfferedProducer, Producer, Publisher, Runtime, RuntimeBuilder,
    SampleContainer, SampleMaybeUninit, SampleMut, ServiceDiscovery, Subscriber, Subscription,
};
use std::collections::{HashMap, VecDeque};
use std::path::{Path, PathBuf};
use std::time::SystemTime;

const SCORE_DEFAULT_SUBSCRIPTION_QUEUE_CAPACITY: usize = 3;
const SCORE_DEFAULT_RECEIVE_BATCH_SIZE: usize = 16;

type LolaRuntime = LolaRuntimeImpl;

type BodyControlModuleServiceInterfaceAlias =
    crate::mw_com_provider_interfaces::vehicle::BodyControlModuleServiceInterface;
type BodyControlModuleServiceProducer =
    <BodyControlModuleServiceInterfaceAlias as Interface>::Producer<LolaRuntime>;
type BodyControlModuleServiceOfferedProducer =
    <BodyControlModuleServiceProducer as Producer<LolaRuntime>>::OfferedProducer;
type BodyControlModuleServiceFrontWasherFluidLevelSubscription = <<LolaRuntime as Runtime>::Subscriber<crate::mw_com_provider_types::vehicle::FrontWasherFluidLevel> as Subscriber<
    crate::mw_com_provider_types::vehicle::FrontWasherFluidLevel,
    LolaRuntime,
>>::Subscription;
type BodyControlModuleServiceFrontWasherFluidIsLowSubscription = <<LolaRuntime as Runtime>::Subscriber<crate::mw_com_provider_types::vehicle::FrontWasherFluidIsLow> as Subscriber<
    crate::mw_com_provider_types::vehicle::FrontWasherFluidIsLow,
    LolaRuntime,
>>::Subscription;
type BodyControlModuleServiceRainIntensitySubscription = <<LolaRuntime as Runtime>::Subscriber<
    crate::mw_com_provider_types::vehicle::RainIntensity,
> as Subscriber<
    crate::mw_com_provider_types::vehicle::RainIntensity,
    LolaRuntime,
>>::Subscription;
type BodyControlModuleServiceDriverDoorChildLockIsActiveSubscription =
    <<LolaRuntime as Runtime>::Subscriber<
        crate::mw_com_provider_types::vehicle::DriverDoorChildLockIsActive,
    > as Subscriber<
        crate::mw_com_provider_types::vehicle::DriverDoorChildLockIsActive,
        LolaRuntime,
    >>::Subscription;
type BodyControlModuleServiceVehicleSpeedSubscription = <<LolaRuntime as Runtime>::Subscriber<
    crate::mw_com_provider_types::vehicle::VehicleSpeed,
> as Subscriber<
    crate::mw_com_provider_types::vehicle::VehicleSpeed,
    LolaRuntime,
>>::Subscription;

#[allow(dead_code)]
enum GeneratedReceiveSubscription {
    NoReceive,
    BodyControlModuleServiceFrontWasherFluidLevel {
        instance_name: String,
        instance_specifier: String,
        subscription: BodyControlModuleServiceFrontWasherFluidLevelSubscription,
    },
    BodyControlModuleServiceFrontWasherFluidIsLow {
        instance_name: String,
        instance_specifier: String,
        subscription: BodyControlModuleServiceFrontWasherFluidIsLowSubscription,
    },
    BodyControlModuleServiceRainIntensity {
        instance_name: String,
        instance_specifier: String,
        subscription: BodyControlModuleServiceRainIntensitySubscription,
    },
    BodyControlModuleServiceDriverDoorChildLockIsActive {
        instance_name: String,
        instance_specifier: String,
        subscription: BodyControlModuleServiceDriverDoorChildLockIsActiveSubscription,
    },
    BodyControlModuleServiceVehicleSpeed {
        instance_name: String,
        instance_specifier: String,
        subscription: BodyControlModuleServiceVehicleSpeedSubscription,
    },
}

impl GeneratedReceiveSubscription {
    fn matches(&self, service_name: &str, instance_specifier: &str, member_name: &str) -> bool {
        match self {
            Self::NoReceive => false,
            Self::BodyControlModuleServiceFrontWasherFluidLevel {
                instance_specifier: subscribed_instance,
                ..
            } => {
                service_name == "BodyControlModuleService"
                    && subscribed_instance == instance_specifier
                    && member_name == "front_washer_fluid_level"
            }
            Self::BodyControlModuleServiceFrontWasherFluidIsLow {
                instance_specifier: subscribed_instance,
                ..
            } => {
                service_name == "BodyControlModuleService"
                    && subscribed_instance == instance_specifier
                    && member_name == "front_washer_fluid_is_low"
            }
            Self::BodyControlModuleServiceRainIntensity {
                instance_specifier: subscribed_instance,
                ..
            } => {
                service_name == "BodyControlModuleService"
                    && subscribed_instance == instance_specifier
                    && member_name == "rain_intensity"
            }
            Self::BodyControlModuleServiceDriverDoorChildLockIsActive {
                instance_specifier: subscribed_instance,
                ..
            } => {
                service_name == "BodyControlModuleService"
                    && subscribed_instance == instance_specifier
                    && member_name == "driver_door_child_lock_is_active"
            }
            Self::BodyControlModuleServiceVehicleSpeed {
                instance_specifier: subscribed_instance,
                ..
            } => {
                service_name == "BodyControlModuleService"
                    && subscribed_instance == instance_specifier
                    && member_name == "vehicle_speed"
            }
        }
    }

    fn unsubscribe(self) {
        match self {
            Self::NoReceive => {}
            Self::BodyControlModuleServiceFrontWasherFluidLevel { subscription, .. } => {
                let _ = subscription.unsubscribe();
            }
            Self::BodyControlModuleServiceFrontWasherFluidIsLow { subscription, .. } => {
                let _ = subscription.unsubscribe();
            }
            Self::BodyControlModuleServiceRainIntensity { subscription, .. } => {
                let _ = subscription.unsubscribe();
            }
            Self::BodyControlModuleServiceDriverDoorChildLockIsActive { subscription, .. } => {
                let _ = subscription.unsubscribe();
            }
            Self::BodyControlModuleServiceVehicleSpeed { subscription, .. } => {
                let _ = subscription.unsubscribe();
            }
        }
    }

    fn try_receive(
        &self,
        pending_messages: &mut VecDeque<MwComMessage>,
        receive_batch_size: usize,
    ) -> Result<(), MwComProviderError> {
        match self {
            Self::NoReceive => Ok(()),
            Self::BodyControlModuleServiceFrontWasherFluidLevel {
                instance_name,
                subscription,
                ..
            } => {
                let mut samples = SampleContainer::new(receive_batch_size);
                let count = subscription
                    .try_receive(&mut samples, receive_batch_size)
                    .map_err(|error| {
                        LolaScoreRuntimeAdapter::score_error(
                            "SCORE front_washer_fluid_level receive failed",
                            error,
                        )
                    })?;
                for _ in 0..count {
                    let sample = samples.pop_front().ok_or_else(|| {
                        MwComProviderError::Transport(
                            "SCORE front_washer_fluid_level receive returned no sample".into(),
                        )
                    })?;
                    pending_messages.push_back(MwComMessage {
                        service: "BodyControlModuleService".into(),
                        instance: instance_name.clone(),
                        member: "front_washer_fluid_level".into(),
                        value: MwComValue::U64(sample.value as u64),
                        source_timestamp: Some(SystemTime::now()),
                        quality: SignalQuality::Valid,
                    });
                }
                Ok(())
            }
            Self::BodyControlModuleServiceFrontWasherFluidIsLow {
                instance_name,
                subscription,
                ..
            } => {
                let mut samples = SampleContainer::new(receive_batch_size);
                let count = subscription
                    .try_receive(&mut samples, receive_batch_size)
                    .map_err(|error| {
                        LolaScoreRuntimeAdapter::score_error(
                            "SCORE front_washer_fluid_is_low receive failed",
                            error,
                        )
                    })?;
                for _ in 0..count {
                    let sample = samples.pop_front().ok_or_else(|| {
                        MwComProviderError::Transport(
                            "SCORE front_washer_fluid_is_low receive returned no sample".into(),
                        )
                    })?;
                    pending_messages.push_back(MwComMessage {
                        service: "BodyControlModuleService".into(),
                        instance: instance_name.clone(),
                        member: "front_washer_fluid_is_low".into(),
                        value: MwComValue::Bool(sample.value),
                        source_timestamp: Some(SystemTime::now()),
                        quality: SignalQuality::Valid,
                    });
                }
                Ok(())
            }
            Self::BodyControlModuleServiceRainIntensity {
                instance_name,
                subscription,
                ..
            } => {
                let mut samples = SampleContainer::new(receive_batch_size);
                let count = subscription
                    .try_receive(&mut samples, receive_batch_size)
                    .map_err(|error| {
                        LolaScoreRuntimeAdapter::score_error(
                            "SCORE rain_intensity receive failed",
                            error,
                        )
                    })?;
                for _ in 0..count {
                    let sample = samples.pop_front().ok_or_else(|| {
                        MwComProviderError::Transport(
                            "SCORE rain_intensity receive returned no sample".into(),
                        )
                    })?;
                    pending_messages.push_back(MwComMessage {
                        service: "BodyControlModuleService".into(),
                        instance: instance_name.clone(),
                        member: "rain_intensity".into(),
                        value: MwComValue::U64(sample.value as u64),
                        source_timestamp: Some(SystemTime::now()),
                        quality: SignalQuality::Valid,
                    });
                }
                Ok(())
            }
            Self::BodyControlModuleServiceDriverDoorChildLockIsActive {
                instance_name,
                subscription,
                ..
            } => {
                let mut samples = SampleContainer::new(receive_batch_size);
                let count = subscription
                    .try_receive(&mut samples, receive_batch_size)
                    .map_err(|error| {
                        LolaScoreRuntimeAdapter::score_error(
                            "SCORE driver_door_child_lock_is_active receive failed",
                            error,
                        )
                    })?;
                for _ in 0..count {
                    let sample = samples.pop_front().ok_or_else(|| {
                        MwComProviderError::Transport(
                            "SCORE driver_door_child_lock_is_active receive returned no sample"
                                .into(),
                        )
                    })?;
                    pending_messages.push_back(MwComMessage {
                        service: "BodyControlModuleService".into(),
                        instance: instance_name.clone(),
                        member: "driver_door_child_lock_is_active".into(),
                        value: MwComValue::Bool(sample.value),
                        source_timestamp: Some(SystemTime::now()),
                        quality: SignalQuality::Valid,
                    });
                }
                Ok(())
            }
            Self::BodyControlModuleServiceVehicleSpeed {
                instance_name,
                subscription,
                ..
            } => {
                let mut samples = SampleContainer::new(receive_batch_size);
                let count = subscription
                    .try_receive(&mut samples, receive_batch_size)
                    .map_err(|error| {
                        LolaScoreRuntimeAdapter::score_error(
                            "SCORE vehicle_speed receive failed",
                            error,
                        )
                    })?;
                for _ in 0..count {
                    let sample = samples.pop_front().ok_or_else(|| {
                        MwComProviderError::Transport(
                            "SCORE vehicle_speed receive returned no sample".into(),
                        )
                    })?;
                    pending_messages.push_back(MwComMessage {
                        service: "BodyControlModuleService".into(),
                        instance: instance_name.clone(),
                        member: "vehicle_speed".into(),
                        value: MwComValue::F64(sample.value as f64),
                        source_timestamp: Some(SystemTime::now()),
                        quality: SignalQuality::Valid,
                    });
                }
                Ok(())
            }
        }
    }
}

pub struct LolaScoreRuntimeAdapter {
    config_path: PathBuf,
    runtime: Option<LolaRuntime>,
    receive_subscriptions: Vec<GeneratedReceiveSubscription>,
    bodycontrolmoduleservice_offered_producers:
        HashMap<String, BodyControlModuleServiceOfferedProducer>,
    pending_messages: VecDeque<MwComMessage>,
    receive_cursor: usize,
}

// SAFETY: The adapter does not expose references to the native LoLa handles it owns, and provider
// access to the transport is serialized through mutable operations on the surrounding lock.
unsafe impl Sync for LolaScoreRuntimeAdapter {}

impl LolaScoreRuntimeAdapter {
    pub fn new(config_path: impl Into<PathBuf>) -> Self {
        let config_path = config_path.into();
        Self {
            config_path,
            runtime: None,
            receive_subscriptions: Vec::new(),
            bodycontrolmoduleservice_offered_producers: HashMap::new(),
            pending_messages: VecDeque::new(),
            receive_cursor: 0,
        }
    }

    pub fn from_score_config_env() -> Result<Self, MwComProviderError> {
        let config_path = std::env::var_os("SCORE_CONFIG_PATH").ok_or_else(|| {
            MwComProviderError::Config("SCORE_CONFIG_PATH must point to the LoLa config".into())
        })?;
        Ok(Self::new(PathBuf::from(config_path)))
    }

    pub fn config_path(&self) -> &Path {
        &self.config_path
    }

    fn runtime(&self) -> Result<&LolaRuntime, MwComProviderError> {
        self.runtime.as_ref().ok_or_else(|| {
            MwComProviderError::Transport("SCORE LoLa runtime is not connected".into())
        })
    }

    fn validate_binding(binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {
        match binding.service_name {
            "BodyControlModuleService" => Ok(()),
            other => Err(MwComProviderError::Transport(format!(
                "LolaScoreRuntimeAdapter is not generated for SCORE service {other}"
            ))),
        }
    }

    fn instance_specifier(
        binding: ScoreBinding<'_>,
    ) -> Result<InstanceSpecifier, MwComProviderError> {
        InstanceSpecifier::new(binding.instance_specifier).map_err(|error| {
            MwComProviderError::Transport(format!(
                "invalid SCORE instance specifier {}: {error:?}",
                binding.instance_specifier
            ))
        })
    }

    fn score_error(context: &str, error: impl std::fmt::Debug) -> MwComProviderError {
        MwComProviderError::Transport(format!("{context}: {error:?}"))
    }

    fn subscription_queue_capacity() -> usize {
        std::env::var("SCORE_SUBSCRIPTION_QUEUE_CAPACITY")
            .ok()
            .and_then(|value| value.parse::<usize>().ok())
            .filter(|value| *value > 0)
            .unwrap_or(SCORE_DEFAULT_SUBSCRIPTION_QUEUE_CAPACITY)
    }

    fn receive_batch_size() -> usize {
        std::env::var("SCORE_RECEIVE_BATCH_SIZE")
            .ok()
            .and_then(|value| value.parse::<usize>().ok())
            .filter(|value| *value > 0)
            .unwrap_or(SCORE_DEFAULT_RECEIVE_BATCH_SIZE)
            .min(Self::subscription_queue_capacity())
    }

    fn ensure_bodycontrolmoduleservice_offered_producer(
        &mut self,
        binding: ScoreBinding<'_>,
    ) -> Result<&BodyControlModuleServiceOfferedProducer, MwComProviderError> {
        if binding.service_name != "BodyControlModuleService" {
            return Err(MwComProviderError::Transport(format!(
                "BodyControlModuleService producer cannot publish {}",
                binding.service_name
            )));
        }

        let instance_specifier = binding.instance_specifier.to_string();
        if !self
            .bodycontrolmoduleservice_offered_producers
            .contains_key(&instance_specifier)
        {
            let specifier = Self::instance_specifier(binding)?;
            let producer = self
                .runtime()?
                .producer_builder::<BodyControlModuleServiceInterfaceAlias>(specifier)
                .build()
                .map_err(|error| Self::score_error("SCORE producer build failed", error))?;
            let offered = producer
                .offer()
                .map_err(|error| Self::score_error("SCORE producer offer failed", error))?;
            self.bodycontrolmoduleservice_offered_producers
                .insert(instance_specifier.clone(), offered);
        }

        self.bodycontrolmoduleservice_offered_producers
            .get(&instance_specifier)
            .ok_or_else(|| MwComProviderError::Transport("SCORE producer was not offered".into()))
    }
}

#[async_trait]
impl ScoreRuntimeAdapter for LolaScoreRuntimeAdapter {
    async fn connect(&mut self) -> Result<(), MwComProviderError> {
        if self.runtime.is_some() {
            return Ok(());
        }
        let mut builder = LolaRuntimeBuilderImpl::new();
        builder.load_config(&self.config_path);
        let runtime = builder
            .build()
            .map_err(|error| Self::score_error("SCORE LoLa runtime build failed", error))?;
        self.runtime = Some(runtime);
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "horn_is_active",
            element_kind: "event",
            payload_type: "HornIsActive",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "low_beam_is_on",
            element_kind: "event",
            payload_type: "LowBeamIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "high_beam_is_on",
            element_kind: "event",
            payload_type: "HighBeamIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "high_beam_switch_is_on",
            element_kind: "event",
            payload_type: "HighBeamSwitchIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "parking_lights_are_on",
            element_kind: "event",
            payload_type: "ParkingLightsAreOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "running_lights_are_on",
            element_kind: "event",
            payload_type: "RunningLightsAreOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_fog_lights_are_on",
            element_kind: "event",
            payload_type: "FrontFogLightsAreOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_fog_lights_are_on",
            element_kind: "event",
            payload_type: "RearFogLightsAreOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "hazard_lights_are_signaling",
            element_kind: "event",
            payload_type: "HazardLightsAreSignaling",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "left_indicator_is_signaling",
            element_kind: "event",
            payload_type: "LeftIndicatorIsSignaling",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "right_indicator_is_signaling",
            element_kind: "event",
            payload_type: "RightIndicatorIsSignaling",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "brake_lights_are_active",
            element_kind: "event",
            payload_type: "BrakeLightsAreActive",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "hood_is_open",
            element_kind: "event",
            payload_type: "HoodIsOpen",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_trunk_is_open",
            element_kind: "event",
            payload_type: "RearTrunkIsOpen",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_trunk_is_locked",
            element_kind: "event",
            payload_type: "RearTrunkIsLocked",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_trunk_position",
            element_kind: "event",
            payload_type: "RearTrunkPosition",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "driver_mirror_is_folded",
            element_kind: "event",
            payload_type: "DriverMirrorIsFolded",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "passenger_mirror_is_folded",
            element_kind: "event",
            payload_type: "PassengerMirrorIsFolded",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "driver_mirror_heating_is_on",
            element_kind: "event",
            payload_type: "DriverMirrorHeatingIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "passenger_mirror_heating_is_on",
            element_kind: "event",
            payload_type: "PassengerMirrorHeatingIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_wiper_mode",
            element_kind: "event",
            payload_type: "FrontWiperMode",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_wiper_intensity",
            element_kind: "event",
            payload_type: "FrontWiperIntensity",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_windshield_heating_is_on",
            element_kind: "event",
            payload_type: "FrontWindshieldHeatingIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_windshield_heating_is_on",
            element_kind: "event",
            payload_type: "RearWindshieldHeatingIsOn",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "driver_door_is_locked",
            element_kind: "event",
            payload_type: "DriverDoorIsLocked",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "driver_door_is_open",
            element_kind: "event",
            payload_type: "DriverDoorIsOpen",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "driver_door_window_position",
            element_kind: "event",
            payload_type: "DriverDoorWindowPosition",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_passenger_door_is_locked",
            element_kind: "event",
            payload_type: "FrontPassengerDoorIsLocked",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_passenger_door_is_open",
            element_kind: "event",
            payload_type: "FrontPassengerDoorIsOpen",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "front_passenger_door_window_position",
            element_kind: "event",
            payload_type: "FrontPassengerDoorWindowPosition",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_driver_door_is_locked",
            element_kind: "event",
            payload_type: "RearDriverDoorIsLocked",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_driver_door_is_open",
            element_kind: "event",
            payload_type: "RearDriverDoorIsOpen",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_passenger_door_is_locked",
            element_kind: "event",
            payload_type: "RearPassengerDoorIsLocked",
        })?;
        self.ensure_bodycontrolmoduleservice_offered_producer(ScoreBinding {
            service_name: "BodyControlModuleService",
            instance_name: "standard_vehicle",
            instance_specifier: "/standard_vehicle_actuators",
            member_name: "rear_passenger_door_is_open",
            element_kind: "event",
            payload_type: "RearPassengerDoorIsOpen",
        })?;
        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), MwComProviderError> {
        for subscription in self.receive_subscriptions.drain(..) {
            subscription.unsubscribe();
        }
        for (_instance_specifier, producer) in
            self.bodycontrolmoduleservice_offered_producers.drain()
        {
            producer
                .unoffer()
                .map_err(|error| Self::score_error("SCORE producer unoffer failed", error))?;
        }
        self.pending_messages.clear();
        self.receive_cursor = 0;
        self.runtime = None;
        Ok(())
    }

    async fn find_service(&self, binding: ScoreBinding<'_>) -> Result<bool, MwComProviderError> {
        Self::validate_binding(binding)?;
        match binding.service_name {
            "BodyControlModuleService" => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let instances = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?;
                Ok(instances.into_iter().next().is_some())
            }
            other => Err(MwComProviderError::Transport(format!(
                "LolaScoreRuntimeAdapter is not generated for SCORE service {other}"
            ))),
        }
    }

    async fn subscribe(&mut self, binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {
        Self::validate_binding(binding)?;
        let queue_capacity = Self::subscription_queue_capacity();
        let instance_specifier = binding.instance_specifier.to_string();
        if self.receive_subscriptions.iter().any(|subscription| {
            subscription.matches(
                binding.service_name,
                &instance_specifier,
                binding.member_name,
            )
        }) {
            return Ok(());
        }

        match (binding.service_name, binding.member_name) {
            ("BodyControlModuleService", "front_washer_fluid_level") => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let consumer_builder = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport(format!(
                            "SCORE service {} at {} is not available",
                            binding.service_name, binding.instance_specifier
                        ))
                    })?;
                let consumer = consumer_builder
                    .build()
                    .map_err(|error| Self::score_error("SCORE consumer build failed", error))?;
                let subscription = consumer
                    .front_washer_fluid_level
                    .subscribe(queue_capacity)
                    .map_err(|error| {
                        Self::score_error("SCORE front_washer_fluid_level subscribe failed", error)
                    })?;
                self.receive_subscriptions.push(
                    GeneratedReceiveSubscription::BodyControlModuleServiceFrontWasherFluidLevel {
                        instance_name: binding.instance_name.to_string(),
                        instance_specifier,
                        subscription,
                    },
                );
                Ok(())
            }
            ("BodyControlModuleService", "front_washer_fluid_is_low") => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let consumer_builder = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport(format!(
                            "SCORE service {} at {} is not available",
                            binding.service_name, binding.instance_specifier
                        ))
                    })?;
                let consumer = consumer_builder
                    .build()
                    .map_err(|error| Self::score_error("SCORE consumer build failed", error))?;
                let subscription = consumer
                    .front_washer_fluid_is_low
                    .subscribe(queue_capacity)
                    .map_err(|error| {
                        Self::score_error("SCORE front_washer_fluid_is_low subscribe failed", error)
                    })?;
                self.receive_subscriptions.push(
                    GeneratedReceiveSubscription::BodyControlModuleServiceFrontWasherFluidIsLow {
                        instance_name: binding.instance_name.to_string(),
                        instance_specifier,
                        subscription,
                    },
                );
                Ok(())
            }
            ("BodyControlModuleService", "rain_intensity") => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let consumer_builder = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport(format!(
                            "SCORE service {} at {} is not available",
                            binding.service_name, binding.instance_specifier
                        ))
                    })?;
                let consumer = consumer_builder
                    .build()
                    .map_err(|error| Self::score_error("SCORE consumer build failed", error))?;
                let subscription =
                    consumer
                        .rain_intensity
                        .subscribe(queue_capacity)
                        .map_err(|error| {
                            Self::score_error("SCORE rain_intensity subscribe failed", error)
                        })?;
                self.receive_subscriptions.push(
                    GeneratedReceiveSubscription::BodyControlModuleServiceRainIntensity {
                        instance_name: binding.instance_name.to_string(),
                        instance_specifier,
                        subscription,
                    },
                );
                Ok(())
            }
            ("BodyControlModuleService", "driver_door_child_lock_is_active") => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let consumer_builder = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport(format!(
                            "SCORE service {} at {} is not available",
                            binding.service_name, binding.instance_specifier
                        ))
                    })?;
                let consumer = consumer_builder
                    .build()
                    .map_err(|error| Self::score_error("SCORE consumer build failed", error))?;
                let subscription = consumer
                    .driver_door_child_lock_is_active
                    .subscribe(queue_capacity)
                    .map_err(|error| {
                        Self::score_error(
                            "SCORE driver_door_child_lock_is_active subscribe failed",
                            error,
                        )
                    })?;
                self.receive_subscriptions.push(
                    GeneratedReceiveSubscription::BodyControlModuleServiceDriverDoorChildLockIsActive {
                        instance_name: binding.instance_name.to_string(),
                        instance_specifier,
                        subscription,
                    },
                );
                Ok(())
            }
            ("BodyControlModuleService", "vehicle_speed") => {
                let discovery = self
                    .runtime()?
                    .find_service::<BodyControlModuleServiceInterfaceAlias>(
                        FindServiceSpecifier::Specific(Self::instance_specifier(binding)?),
                    );
                let consumer_builder = discovery
                    .get_available_instances()
                    .map_err(|error| Self::score_error("SCORE service discovery failed", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport(format!(
                            "SCORE service {} at {} is not available",
                            binding.service_name, binding.instance_specifier
                        ))
                    })?;
                let consumer = consumer_builder
                    .build()
                    .map_err(|error| Self::score_error("SCORE consumer build failed", error))?;
                let subscription =
                    consumer
                        .vehicle_speed
                        .subscribe(queue_capacity)
                        .map_err(|error| {
                            Self::score_error("SCORE vehicle_speed subscribe failed", error)
                        })?;
                self.receive_subscriptions.push(
                    GeneratedReceiveSubscription::BodyControlModuleServiceVehicleSpeed {
                        instance_name: binding.instance_name.to_string(),
                        instance_specifier,
                        subscription,
                    },
                );
                Ok(())
            }
            _ => Ok(()),
        }
    }

    async fn unsubscribe(&mut self, binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {
        Self::validate_binding(binding)?;
        if let Some(index) = self.receive_subscriptions.iter().position(|subscription| {
            subscription.matches(
                binding.service_name,
                binding.instance_specifier,
                binding.member_name,
            )
        }) {
            self.receive_subscriptions.remove(index).unsubscribe();
        }
        if !self.receive_subscriptions.is_empty() {
            self.receive_cursor %= self.receive_subscriptions.len();
        } else {
            self.receive_cursor = 0;
        }
        Ok(())
    }

    async fn recv(&mut self) -> Result<Option<MwComMessage>, MwComProviderError> {
        if let Some(message) = self.pending_messages.pop_front() {
            return Ok(Some(message));
        }

        let subscription_count = self.receive_subscriptions.len();
        if subscription_count == 0 {
            return Ok(None);
        }
        let receive_batch_size = Self::receive_batch_size();

        for _ in 0..subscription_count {
            let index = self.receive_cursor % subscription_count;
            self.receive_subscriptions[index]
                .try_receive(&mut self.pending_messages, receive_batch_size)?;
            self.receive_cursor = (self.receive_cursor + 1) % subscription_count;
            if let Some(message) = self.pending_messages.pop_front() {
                return Ok(Some(message));
            }
        }

        Ok(None)
    }

    async fn send_actuation(
        &mut self,
        binding: ScoreBinding<'_>,
        message: MwComMessage,
    ) -> Result<(), MwComProviderError> {
        match (binding.service_name, binding.member_name) {
            ("BodyControlModuleService", "horn_is_active") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE horn_is_active actuation sample publishing");
                let uninit_sample = producer.horn_is_active.allocate().map_err(|error| {
                    Self::score_error("SCORE horn_is_active allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::HornIsActive { value: value });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE horn_is_active publish failed", error)
                })
            }
            ("BodyControlModuleService", "low_beam_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE low_beam_is_on actuation sample publishing");
                let uninit_sample = producer.low_beam_is_on.allocate().map_err(|error| {
                    Self::score_error("SCORE low_beam_is_on allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::LowBeamIsOn { value: value });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE low_beam_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "high_beam_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE high_beam_is_on actuation sample publishing");
                let uninit_sample = producer.high_beam_is_on.allocate().map_err(|error| {
                    Self::score_error("SCORE high_beam_is_on allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::HighBeamIsOn { value: value });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE high_beam_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "high_beam_switch_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE high_beam_switch_is_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .high_beam_switch_is_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error("SCORE high_beam_switch_is_on allocate failed", error)
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::HighBeamSwitchIsOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE high_beam_switch_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "parking_lights_are_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE parking_lights_are_on actuation sample publishing");
                let uninit_sample = producer.parking_lights_are_on.allocate().map_err(|error| {
                    Self::score_error("SCORE parking_lights_are_on allocate failed", error)
                })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::ParkingLightsAreOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE parking_lights_are_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "running_lights_are_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE running_lights_are_on actuation sample publishing");
                let uninit_sample = producer.running_lights_are_on.allocate().map_err(|error| {
                    Self::score_error("SCORE running_lights_are_on allocate failed", error)
                })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RunningLightsAreOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE running_lights_are_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_fog_lights_are_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_fog_lights_are_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .front_fog_lights_are_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE front_fog_lights_are_on allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontFogLightsAreOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_fog_lights_are_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_fog_lights_are_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_fog_lights_are_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_fog_lights_are_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error("SCORE rear_fog_lights_are_on allocate failed", error)
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearFogLightsAreOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_fog_lights_are_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "hazard_lights_are_signaling") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE hazard_lights_are_signaling actuation sample publishing");
                let uninit_sample =
                    producer
                        .hazard_lights_are_signaling
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE hazard_lights_are_signaling allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::HazardLightsAreSignaling {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE hazard_lights_are_signaling publish failed", error)
                })
            }
            ("BodyControlModuleService", "left_indicator_is_signaling") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE left_indicator_is_signaling actuation sample publishing");
                let uninit_sample =
                    producer
                        .left_indicator_is_signaling
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE left_indicator_is_signaling allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::LeftIndicatorIsSignaling {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE left_indicator_is_signaling publish failed", error)
                })
            }
            ("BodyControlModuleService", "right_indicator_is_signaling") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE right_indicator_is_signaling actuation sample publishing");
                let uninit_sample =
                    producer
                        .right_indicator_is_signaling
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE right_indicator_is_signaling allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RightIndicatorIsSignaling {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE right_indicator_is_signaling publish failed", error)
                })
            }
            ("BodyControlModuleService", "brake_lights_are_active") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE brake_lights_are_active actuation sample publishing");
                let uninit_sample =
                    producer
                        .brake_lights_are_active
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE brake_lights_are_active allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::BrakeLightsAreActive { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE brake_lights_are_active publish failed", error)
                })
            }
            ("BodyControlModuleService", "hood_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE hood_is_open actuation sample publishing");
                let uninit_sample = producer.hood_is_open.allocate().map_err(|error| {
                    Self::score_error("SCORE hood_is_open allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::HoodIsOpen { value: value });
                sample
                    .send()
                    .map_err(|error| Self::score_error("SCORE hood_is_open publish failed", error))
            }
            ("BodyControlModuleService", "rear_trunk_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_trunk_is_open actuation sample publishing");
                let uninit_sample = producer.rear_trunk_is_open.allocate().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_is_open allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::RearTrunkIsOpen { value: value });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_is_open publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_trunk_is_locked") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_trunk_is_locked actuation sample publishing");
                let uninit_sample = producer.rear_trunk_is_locked.allocate().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_is_locked allocate failed", error)
                })?;
                let sample =
                    uninit_sample.write(crate::mw_com_provider_types::vehicle::RearTrunkIsLocked {
                        value: value,
                    });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_is_locked publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_trunk_position") => {
                let value = numeric_value(&message.value, binding).map(|value| value as u8)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_trunk_position actuation sample publishing");
                let uninit_sample = producer.rear_trunk_position.allocate().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_position allocate failed", error)
                })?;
                let sample =
                    uninit_sample.write(crate::mw_com_provider_types::vehicle::RearTrunkPosition {
                        value: value,
                    });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_trunk_position publish failed", error)
                })
            }
            ("BodyControlModuleService", "driver_mirror_is_folded") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE driver_mirror_is_folded actuation sample publishing");
                let uninit_sample =
                    producer
                        .driver_mirror_is_folded
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE driver_mirror_is_folded allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::DriverMirrorIsFolded { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE driver_mirror_is_folded publish failed", error)
                })
            }
            ("BodyControlModuleService", "passenger_mirror_is_folded") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE passenger_mirror_is_folded actuation sample publishing");
                let uninit_sample =
                    producer
                        .passenger_mirror_is_folded
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE passenger_mirror_is_folded allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::PassengerMirrorIsFolded { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE passenger_mirror_is_folded publish failed", error)
                })
            }
            ("BodyControlModuleService", "driver_mirror_heating_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE driver_mirror_heating_is_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .driver_mirror_heating_is_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE driver_mirror_heating_is_on allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::DriverMirrorHeatingIsOn { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE driver_mirror_heating_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "passenger_mirror_heating_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE passenger_mirror_heating_is_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .passenger_mirror_heating_is_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE passenger_mirror_heating_is_on allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::PassengerMirrorHeatingIsOn {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE passenger_mirror_heating_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_wiper_mode") => {
                let value = crate::score_bindings::encode_string16(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_wiper_mode actuation sample publishing");
                let uninit_sample = producer.front_wiper_mode.allocate().map_err(|error| {
                    Self::score_error("SCORE front_wiper_mode allocate failed", error)
                })?;
                let sample = uninit_sample
                    .write(crate::mw_com_provider_types::vehicle::FrontWiperMode { value: value });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_wiper_mode publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_wiper_intensity") => {
                let value = numeric_value(&message.value, binding).map(|value| value as u8)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_wiper_intensity actuation sample publishing");
                let uninit_sample = producer.front_wiper_intensity.allocate().map_err(|error| {
                    Self::score_error("SCORE front_wiper_intensity allocate failed", error)
                })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontWiperIntensity { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_wiper_intensity publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_windshield_heating_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_windshield_heating_is_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .front_windshield_heating_is_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE front_windshield_heating_is_on allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontWindshieldHeatingIsOn {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_windshield_heating_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_windshield_heating_is_on") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_windshield_heating_is_on actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_windshield_heating_is_on
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE rear_windshield_heating_is_on allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearWindshieldHeatingIsOn {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_windshield_heating_is_on publish failed", error)
                })
            }
            ("BodyControlModuleService", "driver_door_is_locked") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE driver_door_is_locked actuation sample publishing");
                let uninit_sample = producer.driver_door_is_locked.allocate().map_err(|error| {
                    Self::score_error("SCORE driver_door_is_locked allocate failed", error)
                })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::DriverDoorIsLocked { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE driver_door_is_locked publish failed", error)
                })
            }
            ("BodyControlModuleService", "driver_door_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE driver_door_is_open actuation sample publishing");
                let uninit_sample = producer.driver_door_is_open.allocate().map_err(|error| {
                    Self::score_error("SCORE driver_door_is_open allocate failed", error)
                })?;
                let sample =
                    uninit_sample.write(crate::mw_com_provider_types::vehicle::DriverDoorIsOpen {
                        value: value,
                    });
                sample.send().map_err(|error| {
                    Self::score_error("SCORE driver_door_is_open publish failed", error)
                })
            }
            ("BodyControlModuleService", "driver_door_window_position") => {
                let value = numeric_value(&message.value, binding).map(|value| value as u8)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE driver_door_window_position actuation sample publishing");
                let uninit_sample =
                    producer
                        .driver_door_window_position
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE driver_door_window_position allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::DriverDoorWindowPosition {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE driver_door_window_position publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_passenger_door_is_locked") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_passenger_door_is_locked actuation sample publishing");
                let uninit_sample =
                    producer
                        .front_passenger_door_is_locked
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE front_passenger_door_is_locked allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontPassengerDoorIsLocked {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_passenger_door_is_locked publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_passenger_door_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_passenger_door_is_open actuation sample publishing");
                let uninit_sample =
                    producer
                        .front_passenger_door_is_open
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE front_passenger_door_is_open allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontPassengerDoorIsOpen {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE front_passenger_door_is_open publish failed", error)
                })
            }
            ("BodyControlModuleService", "front_passenger_door_window_position") => {
                let value = numeric_value(&message.value, binding).map(|value| value as u8)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE front_passenger_door_window_position actuation sample publishing");
                let uninit_sample = producer
                    .front_passenger_door_window_position
                    .allocate()
                    .map_err(|error| {
                        Self::score_error(
                            "SCORE front_passenger_door_window_position allocate failed",
                            error,
                        )
                    })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::FrontPassengerDoorWindowPosition {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error(
                        "SCORE front_passenger_door_window_position publish failed",
                        error,
                    )
                })
            }
            ("BodyControlModuleService", "rear_driver_door_is_locked") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_driver_door_is_locked actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_driver_door_is_locked
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE rear_driver_door_is_locked allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearDriverDoorIsLocked { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_driver_door_is_locked publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_driver_door_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_driver_door_is_open actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_driver_door_is_open
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE rear_driver_door_is_open allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearDriverDoorIsOpen { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_driver_door_is_open publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_passenger_door_is_locked") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_passenger_door_is_locked actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_passenger_door_is_locked
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE rear_passenger_door_is_locked allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearPassengerDoorIsLocked {
                        value: value,
                    },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_passenger_door_is_locked publish failed", error)
                })
            }
            ("BodyControlModuleService", "rear_passenger_door_is_open") => {
                let value = bool_value(&message.value, binding)?;
                let producer = self.ensure_bodycontrolmoduleservice_offered_producer(binding)?;
                tracing::debug!(value = ?value, "SCORE rear_passenger_door_is_open actuation sample publishing");
                let uninit_sample =
                    producer
                        .rear_passenger_door_is_open
                        .allocate()
                        .map_err(|error| {
                            Self::score_error(
                                "SCORE rear_passenger_door_is_open allocate failed",
                                error,
                            )
                        })?;
                let sample = uninit_sample.write(
                    crate::mw_com_provider_types::vehicle::RearPassengerDoorIsOpen { value: value },
                );
                sample.send().map_err(|error| {
                    Self::score_error("SCORE rear_passenger_door_is_open publish failed", error)
                })
            }
            (_, other) => Err(MwComProviderError::Transport(format!(
                "SCORE actuation is not generated for {}:{}",
                binding.service_name, other
            ))),
        }
    }
}

fn score_quality(value: u8) -> Result<SignalQuality, MwComProviderError> {
    match value {
        0 => Ok(SignalQuality::Invalid),
        1 => Ok(SignalQuality::Stale),
        2 => Ok(SignalQuality::Degraded),
        3 => Ok(SignalQuality::Valid),
        other => Err(MwComProviderError::Transport(format!(
            "unsupported SCORE quality value {other}"
        ))),
    }
}

#[allow(dead_code)]
fn numeric_value(value: &MwComValue, binding: ScoreBinding<'_>) -> Result<f64, MwComProviderError> {
    match value {
        MwComValue::F64(value) => Ok(*value),
        MwComValue::I64(value) => Ok(*value as f64),
        MwComValue::U64(value) => Ok(*value as f64),
        other => Err(MwComProviderError::Transport(format!(
            "{}:{} requires numeric actuation, got {other:?}",
            binding.service_name, binding.member_name
        ))),
    }
}

#[allow(dead_code)]
fn bool_value(value: &MwComValue, binding: ScoreBinding<'_>) -> Result<bool, MwComProviderError> {
    match value {
        MwComValue::Bool(value) => Ok(*value),
        other => Err(MwComProviderError::Transport(format!(
            "{}:{} requires bool actuation, got {other:?}",
            binding.service_name, binding.member_name
        ))),
    }
}
