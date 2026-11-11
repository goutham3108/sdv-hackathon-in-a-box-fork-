/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

use com_api_gen::{
    BodyControlModuleServiceInterface, DriverDoorChildLockIsActive, FrontWasherFluidIsLow,
    FrontWasherFluidLevel, HornIsActive, RainIntensity, VehicleSpeed,
};
use score_com::{
    Builder, FindServiceSpecifier, InstanceSpecifier, Interface, OfferedProducer, Producer,
    Publisher, Result, Runtime, SampleContainer, SampleMaybeUninit, SampleMut, ServiceDiscovery,
    Subscriber, Subscription,
};
use score_log as log;
use std::thread;
use std::time::Duration;

type BodyControlOfferedProducer<R> =
    <<BodyControlModuleServiceInterface as Interface>::Producer<R> as Producer<R>>::OfferedProducer;
type HornSubscription<R> =
    <<R as Runtime>::Subscriber<HornIsActive> as Subscriber<HornIsActive, R>>::Subscription;

pub struct BodyControlModuleProducer<R: Runtime> {
    producer: BodyControlOfferedProducer<R>,
}

impl<R: Runtime> BodyControlModuleProducer<R> {
    pub fn new(runtime: &R, instance: InstanceSpecifier) -> Result<Self> {
        let producer = runtime
            .producer_builder::<BodyControlModuleServiceInterface>(instance)
            .build()?
            .offer()?;
        Ok(Self { producer })
    }

    fn publish_sample(&self, sequence: usize) -> Result<()> {
        self.producer
            .front_washer_fluid_level
            .allocate()?
            .write(FrontWasherFluidLevel {
                value: (sequence % 101) as u8,
            })
            .send()?;
        self.producer
            .front_washer_fluid_is_low
            .allocate()?
            .write(FrontWasherFluidIsLow {
                value: sequence % 2 == 0,
            })
            .send()?;
        self.producer
            .rain_intensity
            .allocate()?
            .write(RainIntensity {
                value: (sequence % 101) as u8,
            })
            .send()?;
        self.producer
            .driver_door_child_lock_is_active
            .allocate()?
            .write(DriverDoorChildLockIsActive {
                value: sequence % 2 != 0,
            })
            .send()?;
        self.producer
            .vehicle_speed
            .allocate()?
            .write(VehicleSpeed {
                value: 50.0 + sequence as f32,
            })
            .send()?;
        Ok(())
    }

    pub fn run_publish_loop(&self, count: usize) {
        for sequence in 0..count {
            if let Err(error) = self.publish_sample(sequence) {
                log::error!(
                    "Failed to publish BodyControlModule datapoints: {:?}",
                    error
                );
            }
            thread::sleep(Duration::from_millis(1000));
        }
    }

    pub fn unoffer(self) {
        if let Err(error) = self.producer.unoffer() {
            log::error!("Failed to unoffer BodyControlModuleService: {:?}", error);
        }
    }
}

pub struct BodyControlModuleConsumer<R: Runtime> {
    horn_subscription: HornSubscription<R>,
}

impl<R: Runtime> BodyControlModuleConsumer<R> {
    pub fn find_available_instance(
        runtime: &R,
        instance: InstanceSpecifier,
    ) -> Result<Option<Self>> {
        let discovery = runtime.find_service::<BodyControlModuleServiceInterface>(
            FindServiceSpecifier::Specific(instance),
        );
        let Some(builder) = discovery.get_available_instances()?.into_iter().next() else {
            return Ok(None);
        };
        let consumer = builder.build()?;
        let horn_subscription = consumer.horn_is_active.subscribe(3)?;
        Ok(Some(Self { horn_subscription }))
    }

    pub fn read_horn_actuations(&self, count: usize) {
        let mut samples = SampleContainer::new(3);
        for _ in 0..count {
            match self.horn_subscription.try_receive(&mut samples, 1) {
                Ok(0) => log::info!("No horn actuation received"),
                Ok(received) => {
                    if let Some(sample) = samples.pop_front() {
                        log::info!(
                            "{} horn actuation samples received: {}",
                            received,
                            sample.value
                        );
                    }
                }
                Err(error) => log::error!("Horn actuation receive failed: {:?}", error),
            }
            thread::sleep(Duration::from_millis(1000));
        }
    }

    pub fn unsubscribe(self) {
        let _ = self.horn_subscription.unsubscribe();
    }
}
