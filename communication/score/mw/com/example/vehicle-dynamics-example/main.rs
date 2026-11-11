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

//! Vehicle Dynamics mw::com example.
//!
//! The producer offers `VehicleDynamicsService` and publishes `SpeedSample`
//! values through the `speed` event.
//!
//! The consumer discovers `/vehicle/front_vehicle`, subscribes to `speed`,
//! and prints the received speed and quality values.

use clap::Parser;
use std::path::PathBuf;
use std::sync::Arc;
use std::thread;
use std::time::Duration;

use score_com::{
    Builder,
    InstanceSpecifier,
    LolaRuntimeBuilderImpl,
    Runtime,
    RuntimeBuilder,
};
use score_log as log;
use stdout_logger::StdoutLoggerBuilder;

use vehicle_dynamics_example::{
    VehicleMonitorConsumer,
    VehicleMonitorProducer,
};
/// Controls which roles are started in this process.
#[derive(clap::ValueEnum, Clone, Debug, Default)]
enum RunMode {
    /// Start both producer and consumer.
    #[default]
    Both,
    /// Start only the producer.
    Producer,
    /// Start only the consumer.
    Consumer,
}

#[derive(Parser)]
struct Arguments {
    /// Select producer, consumer, or both.
    #[arg(short, long, value_enum, default_value = "both")]
    run_mode: RunMode,

    /// SCORE mw::com service-instance manifest.
    #[arg(
        short,
        long,
        default_value = "./score/mw/com/example/vehicle-dynamics-example/etc/mw_com_config.json"
    )]
    service_instance_manifest: PathBuf,
}

/// SCORE instance specifier shared by the producer and consumer.
const VEHICLE_INSTANCE_SPECIFIER: &str = //"/vehicle/front_vehicle";
"/Vehicle/Service1/Instance";

/// Number of speed samples to publish and receive.
const SAMPLE_COUNT: usize = 100000;

/// Initial speed value published by the producer.
const INITIAL_SPEED: f64 = 50.0;

/// Delay before the consumer performs synchronous discovery.
const CONSUMER_STARTUP_DELAY_MS: u64 = 500;

/// Runs the Vehicle Dynamics consumer.
fn run_consumer<R: Runtime>(runtime_name: &str, runtime: &R) {
    log::info!("Starting Vehicle Dynamics consumer with {} runtime", runtime_name);

    let service_id = InstanceSpecifier::new(VEHICLE_INSTANCE_SPECIFIER)
        .expect("Failed to create Vehicle Dynamics instance specifier");

    // In both-mode, give the producer time to offer the service.
    thread::sleep(Duration::from_millis(
        CONSUMER_STARTUP_DELAY_MS,
    ));

    let consumer_monitor =
        VehicleMonitorConsumer::find_available_instances(runtime, service_id)
            .expect(
                "Failed to discover VehicleDynamicsService at \
                 /vehicle/front_vehicle",
            );

    log::info!("Vehicle Dynamics consumer created successfully");

    consumer_monitor
        .read_speed_data(SAMPLE_COUNT)
        .expect("Failed to receive speed samples");

    consumer_monitor.unsubscribe();

    log::info!("Vehicle Dynamics consumer completed");
}

/// Runs the Vehicle Dynamics producer.
fn run_producer<R: Runtime>(runtime_name: &str, runtime: &R) {
    println!("STEP1");
    log::info!("Starting Vehicle Dynamics producer with {} runtime", runtime_name);

let service_id =
InstanceSpecifier::new("/Vehicle/Service1/Instance")
 .expect("Failed");

    println!("STEP2");
    
    let producer_monitor =
        VehicleMonitorProducer::new(runtime, service_id)
            .expect("Failed to create Vehicle Dynamics producer");
	    
    println!("STEP3");

    log::info!("Vehicle Dynamics producer created and offered successfully");

    producer_monitor.run_publish_loop(
        INITIAL_SPEED,
        SAMPLE_COUNT,
    );

    producer_monitor.unoffer();

    log::info!("Vehicle Dynamics producer completed");
}

/// Creates a LoLa runtime builder using the supplied configuration file.
fn create_lola_runtime_builder(
    config_path: &std::path::Path,
) -> LolaRuntimeBuilderImpl {
    assert!(
        config_path.exists(),
        "Configuration file not found: {}",
        config_path.display()
    );

    let mut builder = LolaRuntimeBuilderImpl::new();
    builder.load_config(config_path);
    builder
}
fn init_logging() {
    StdoutLoggerBuilder::new()
        .show_module(false)
        .show_file(true)
        .show_line(false)
        .set_as_default_logger();
}

fn main() {
    init_logging();

    let args = Arguments::parse();

    let runtime_builder =
        create_lola_runtime_builder(&args.service_instance_manifest);

    let runtime = Arc::new(
        runtime_builder
            .build()
            .expect("Failed to build LoLa runtime"),
    );

    let spawn_producer =
        matches!(args.run_mode, RunMode::Both | RunMode::Producer);

    let spawn_consumer =
        matches!(args.run_mode, RunMode::Both | RunMode::Consumer);

    let producer_runtime = Arc::clone(&runtime);
    let consumer_runtime = Arc::clone(&runtime);

    let producer_handle = spawn_producer.then(|| {
        thread::spawn(move || {
            run_producer("LoLa", producer_runtime.as_ref());
        })
    });

    let consumer_handle = spawn_consumer.then(|| {
        thread::spawn(move || {
            run_consumer("LoLa", consumer_runtime.as_ref());
        })
    });
    
    for handle in [producer_handle, consumer_handle]
    .into_iter()
    .flatten()
    {
        handle.join().expect("Thread panicked");
    }
    log::info!("Vehicle Dynamics example completed");
}
