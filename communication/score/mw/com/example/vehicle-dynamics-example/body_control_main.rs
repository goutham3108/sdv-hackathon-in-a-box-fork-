/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

use clap::Parser;
use score_com::{Builder, InstanceSpecifier, LolaRuntimeBuilderImpl, Runtime, RuntimeBuilder};
use score_log as log;
use std::path::PathBuf;
use std::sync::Arc;
use std::thread;
use std::time::Duration;
use stdout_logger::StdoutLoggerBuilder;
use vehicle_dynamics_example::{BodyControlModuleConsumer, BodyControlModuleProducer};

const DATAPOINT_INSTANCE: &str = "/standard_vehicle_datapoints";
const ACTUATOR_INSTANCE: &str = "/standard_vehicle_actuators";
const SAMPLE_COUNT: usize = 100;

#[derive(clap::ValueEnum, Clone, Debug, Default)]
enum RunMode {
    #[default]
    Both,
    Producer,
    Consumer,
}

#[derive(Parser)]
struct Arguments {
    #[arg(short, long, value_enum, default_value = "both")]
    run_mode: RunMode,

    #[arg(
        short,
        long,
        default_value = "./score/mw/com/example/vehicle-dynamics-example/etc/body_control_module_lola_config.json"
    )]
    service_instance_manifest: PathBuf,
}

fn run_producer<R: Runtime>(runtime: &R) {
    let instance = InstanceSpecifier::new(DATAPOINT_INSTANCE)
        .expect("Failed to create BodyControl datapoint instance specifier");
    let producer = BodyControlModuleProducer::new(runtime, instance)
        .expect("Failed to offer BodyControlModuleService datapoints");
    log::info!("BodyControl datapoint service offered");
    producer.run_publish_loop(SAMPLE_COUNT);
    producer.unoffer();
}

fn run_consumer<R: Runtime>(runtime: &R) {
    let mut consumer = None;
    for _ in 0..100 {
        let instance = InstanceSpecifier::new(ACTUATOR_INSTANCE)
            .expect("Failed to create BodyControl actuator instance specifier");
        match BodyControlModuleConsumer::find_available_instance(runtime, instance) {
            Ok(Some(available)) => {
                consumer = Some(available);
                break;
            }
            Ok(None) => thread::sleep(Duration::from_millis(100)),
            Err(error) => {
                log::warn!("BodyControl actuator discovery failed: {:?}", error);
                thread::sleep(Duration::from_millis(100));
            }
        }
    }

    let Some(consumer) = consumer else {
        log::error!("BodyControl actuator service did not become available");
        return;
    };
    log::info!("BodyControl actuator consumer subscribed to horn_is_active");
    consumer.read_horn_actuations(SAMPLE_COUNT);
    consumer.unsubscribe();
}

fn main() {
    StdoutLoggerBuilder::new()
        .show_module(false)
        .show_file(true)
        .show_line(false)
        .set_as_default_logger();

    let args = Arguments::parse();
    let mut runtime_builder: LolaRuntimeBuilderImpl = LolaRuntimeBuilderImpl::new();
    runtime_builder.load_config(&args.service_instance_manifest);
    let runtime = Arc::new(
        runtime_builder
            .build()
            .expect("Failed to build BodyControl LoLa runtime"),
    );

    let spawn_producer = matches!(args.run_mode, RunMode::Both | RunMode::Producer);
    let spawn_consumer = matches!(args.run_mode, RunMode::Both | RunMode::Consumer);
    let producer_runtime = Arc::clone(&runtime);
    let consumer_runtime = Arc::clone(&runtime);

    let producer =
        spawn_producer.then(|| thread::spawn(move || run_producer(producer_runtime.as_ref())));
    let consumer =
        spawn_consumer.then(|| thread::spawn(move || run_consumer(consumer_runtime.as_ref())));

    for handle in [producer, consumer].into_iter().flatten() {
        handle.join().expect("BodyControl example thread panicked");
    }
}
