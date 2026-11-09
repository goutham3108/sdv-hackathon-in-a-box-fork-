pub mod body_control;
pub mod consumer;
pub mod producer;

pub use body_control::{BodyControlModuleConsumer, BodyControlModuleProducer};
pub use consumer::VehicleMonitorConsumer;
pub use producer::VehicleMonitorProducer;

use score_com::{Interface, Producer};

use com_api_gen::VehicleDynamicsServiceInterface;

pub type VehicleConsumer<R> =
    <VehicleDynamicsServiceInterface as Interface>::Consumer<R>;

pub type VehicleOfferedProducer<R> =
    <<VehicleDynamicsServiceInterface as Interface>::Producer<R> as Producer<R>>::OfferedProducer;