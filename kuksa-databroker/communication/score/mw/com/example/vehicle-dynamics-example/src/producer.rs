/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

use crate::VehicleOfferedProducer;
use score_com::{
    Builder,
    InstanceSpecifier,
    OfferedProducer,
    Producer,
    Publisher,
    Result,
    Runtime,
    SampleMaybeUninit,
    SampleMut,
};

use com_api_gen::{SpeedSample, VehicleDynamicsServiceInterface};

use std::thread;
use std::time::Duration;

use score_log as log;

const PRODUCER_SEND_INTERVAL_MS: u64 = 1000;

pub struct VehicleMonitorProducer<R: Runtime> {
    producer: VehicleOfferedProducer<R>,
}

impl<R: Runtime> VehicleMonitorProducer<R> {
    pub fn new(runtime: &R, service_id: InstanceSpecifier) -> Result<Self> {
    println!("P1");
        let producer_builder =
            runtime.producer_builder::<VehicleDynamicsServiceInterface>(service_id);

    println!("P2");

    let producer = producer_builder.build()?;

    println!("P3");

    let producer = producer.offer()?;

    println!("P4");

        Ok(Self { producer })
    }

    pub fn publish_speed_data(&self, speed: SpeedSample) -> Result<()> {

        log::info!(
            "Speed sample sent: value={}, quality={}",
            speed.value,
            speed.quality
        );
        let uninit_sample = self.producer.speed.allocate()?;

        let sample = uninit_sample.write(speed);

        sample.send()?;
        Ok(())
    }

    pub fn run_publish_loop(&self, initial_speed: f64, count: usize) {
        for i in 0..count {
            let sample = SpeedSample {
                value: initial_speed + i as f64,
                quality: 3,
            };

            match self.publish_speed_data(sample) {
                Ok(_) => {}
                Err(e) => {
                    log::error!(
                        "Failed to publish speed sample: {:?}",
                        e
                    );
                }
            }

            thread::sleep(Duration::from_millis(
                PRODUCER_SEND_INTERVAL_MS,
            ));
        }
    }

    pub fn unoffer(self) {
        match self.producer.unoffer() {
            Ok(_) => {
                log::info!("Successfully unoffered service")
            }
            Err(e) => {
                log::error!("Failed to unoffer: {:?}", e)
            }
        }
    }
}
