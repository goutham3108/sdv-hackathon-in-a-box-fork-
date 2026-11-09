use score_com::{
    ConsumerBuilder,
    FindServiceSpecifier,
    InstanceSpecifier,
    Result,
    Runtime,
    SampleContainer,
    ServiceDiscovery,
    Subscriber,
    Subscription,
};

use com_api_gen::{SpeedSample, VehicleDynamicsServiceInterface};

use std::thread;
use std::time::Duration;

use score_log as log;

const SYNC_RECEIVE_POLL_INTERVAL_MS: u64 = 1000;

pub struct VehicleMonitorConsumer<R: Runtime> {
    speed_subscriber:
        <<R as Runtime>::Subscriber<SpeedSample> as Subscriber<SpeedSample, R>>::Subscription,
}

impl<R: Runtime> VehicleMonitorConsumer<R> {
    fn from_service_instances<B>(instances: impl IntoIterator<Item = B>) -> Result<Self>
    where
        B: ConsumerBuilder<VehicleDynamicsServiceInterface, R>,
    {
        let consumer_builder = instances
            .into_iter()
            .next()
            .expect("Failed to get consumer builder");

        let consumer = consumer_builder.build()?;

        let speed_subscriber = consumer.speed.subscribe(3)?;

        Ok(Self { speed_subscriber })
    }

    pub fn find_available_instances(
        runtime: &R,
        service_id: InstanceSpecifier,
    ) -> Result<Self> {
        let consumer_discovery =
            runtime.find_service::<VehicleDynamicsServiceInterface>(
                FindServiceSpecifier::Specific(service_id),
            );

        let instances = consumer_discovery.get_available_instances()?;

        Self::from_service_instances(instances)
    }

    pub fn read_speed_data(&self, count: usize) -> Result<String> {
        let mut sample_buf = SampleContainer::new(3);

        for _ in 0..count {
            let result =
                self.speed_subscriber.try_receive(&mut sample_buf, 1);

            match result {
                Ok(0) => {
                    log::info!("No speed sample received");
                }
                Ok(x) => {
                    let sample =
                        sample_buf.pop_front().expect("Buffer error");

                    log::info!(
                        "{} samples received. speed={} quality={}",
                        x,
                        sample.value,
                        sample.quality
                    );
                }
                Err(e) => {
                    log::error!("Receive error: {:?}", e);
                }
            }

            thread::sleep(Duration::from_millis(
                SYNC_RECEIVE_POLL_INTERVAL_MS,
            ));
        }

        Ok("All speed samples received".to_string())
    }

    pub fn unsubscribe(self) {
        let _ = self.speed_subscriber.unsubscribe();
    }
}
