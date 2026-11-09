use crate::config::ProviderConfig;
use crate::error::MwComProviderError;
use crate::gateway_samples::{
    BodySignalPayload, BodySignalText, HighBeamCommand, HighBeamState, SpeedAck, SpeedSample,
};
use crate::quality::SignalQuality;
use crate::score_bindings::{ScoreBinding, ScoreRuntimeAdapter};
use crate::transport::{MwComMessage, MwComValue};
use async_trait::async_trait;
use score_com::{
    Builder, CommData, FindServiceSpecifier, InstanceSpecifier, Interface, LolaRuntimeBuilderImpl,
    LolaRuntimeImpl, OfferedProducer, Producer, ProviderInfo, Publisher, Runtime, RuntimeBuilder,
    SampleContainer, ServiceDiscovery, Subscriber, Subscription,
};
use std::fmt::Debug;
use std::path::PathBuf;
use std::time::SystemTime;

type LolaRuntime = LolaRuntimeImpl;
type TxProducer = <HighBeamTxInterface as Interface>::Producer<LolaRuntime>;
type TxOfferedProducer = <TxProducer as Producer<LolaRuntime>>::OfferedProducer;
type BoolSubscription = <<LolaRuntime as Runtime>::Subscriber<HighBeamState> as Subscriber<
    HighBeamState,
    LolaRuntime,
>>::Subscription;
type ByteSubscription = <<LolaRuntime as Runtime>::Subscriber<HighBeamCommand> as Subscriber<
    HighBeamCommand,
    LolaRuntime,
>>::Subscription;
type TextSubscription = <<LolaRuntime as Runtime>::Subscriber<BodySignalText> as Subscriber<
    BodySignalText,
    LolaRuntime,
>>::Subscription;
type PayloadSubscription = <<LolaRuntime as Runtime>::Subscriber<BodySignalPayload> as Subscriber<
    BodySignalPayload,
    LolaRuntime,
>>::Subscription;
type SpeedSubscription = <<LolaRuntime as Runtime>::Subscriber<SpeedSample> as Subscriber<
    SpeedSample,
    LolaRuntime,
>>::Subscription;

enum GatewaySubscription {
    Bool(BoolSubscription),
    Byte(ByteSubscription),
    Text(TextSubscription),
    Payload(PayloadSubscription, &'static str),
    Speed(SpeedSubscription),
}

struct SubscriptionEntry {
    service: String,
    instance: String,
    member: String,
    subscription: GatewaySubscription,
}

impl SubscriptionEntry {
    fn new(binding: ScoreBinding<'_>, subscription: GatewaySubscription) -> Self {
        Self {
            service: binding.service_name.into(),
            instance: binding.instance_name.into(),
            member: binding.member_name.into(),
            subscription,
        }
    }

    fn matches(&self, binding: ScoreBinding<'_>) -> bool {
        self.service == binding.service_name
            && self.instance == binding.instance_name
            && self.member == binding.member_name
    }

    fn unsubscribe(self) {
        match self.subscription {
            GatewaySubscription::Bool(sub) => {
                let _ = sub.unsubscribe();
            }
            GatewaySubscription::Byte(sub) => {
                let _ = sub.unsubscribe();
            }
            GatewaySubscription::Text(sub) => {
                let _ = sub.unsubscribe();
            }
            GatewaySubscription::Payload(sub, _) => {
                let _ = sub.unsubscribe();
            }
            GatewaySubscription::Speed(sub) => {
                let _ = sub.unsubscribe();
            }
        }
    }

    fn poll(&self) -> Result<Option<MwComMessage>, MwComProviderError> {
        let value = match &self.subscription {
            GatewaySubscription::Bool(sub) => receive(sub, |sample: &HighBeamState| {
                Ok((MwComValue::Bool(sample.value), SignalQuality::Valid))
            }),
            GatewaySubscription::Byte(sub) => receive(sub, |sample: &HighBeamCommand| {
                Ok((sample.decode()?, SignalQuality::Valid))
            }),
            GatewaySubscription::Text(sub) => receive(sub, |sample: &BodySignalText| {
                Ok((sample.decode()?, SignalQuality::Valid))
            }),
            GatewaySubscription::Payload(sub, datatype) => {
                receive(sub, |sample: &BodySignalPayload| {
                Ok((sample.decode(datatype)?, SignalQuality::Valid))
                })
            }
            GatewaySubscription::Speed(sub) => receive(sub, |sample: &SpeedSample| {
                let quality = match sample.quality {
                    0 => SignalQuality::Invalid,
                    1 => SignalQuality::Stale,
                    2 => SignalQuality::Degraded,
                    3 => SignalQuality::Valid,
                    _ => {
                        return Err(MwComProviderError::Transport(
                            "invalid gateway speed quality".into(),
                        ));
                    }
                };
                Ok((MwComValue::F64(sample.value), quality))
            }),
        }?;
        Ok(value.map(|(value, quality)| MwComMessage {
            service: self.service.clone(),
            instance: self.instance.clone(),
            member: self.member.clone(),
            value,
            quality,
            source_timestamp: Some(SystemTime::now()),
        }))
    }
}

fn receive<T, S>(
    subscription: &S,
    decode: impl FnOnce(&T) -> Result<(MwComValue, SignalQuality), MwComProviderError>,
) -> Result<Option<(MwComValue, SignalQuality)>, MwComProviderError>
where
    T: CommData + Debug,
    S: Subscription<T, LolaRuntime>,
{
    let mut samples = SampleContainer::new(1);
    let count = subscription
        .try_receive(&mut samples, 1)
        .map_err(|error| GatewayScoreRuntimeAdapter::error("gateway receive", error))?;
    if count == 0 {
        return Ok(None);
    }
    let sample = samples.pop_front().ok_or_else(|| {
        MwComProviderError::Transport("gateway receive returned no sample".into())
    })?;
    Ok(Some(decode(&sample)?))
}

pub struct GatewayScoreRuntimeAdapter {
    config_path: PathBuf,
    runtime: Option<LolaRuntime>,
    tx: Option<TxOfferedProducer>,
    subscriptions: Vec<SubscriptionEntry>,
    cursor: usize,
}

unsafe impl Sync for GatewayScoreRuntimeAdapter {}

impl GatewayScoreRuntimeAdapter {
    pub fn new(config_path: impl Into<PathBuf>) -> Self {
        Self {
            config_path: config_path.into(),
            runtime: None,
            tx: None,
            subscriptions: Vec::new(),
            cursor: 0,
        }
    }

    pub fn supports(config: &ProviderConfig) -> bool {
        !config.mappings.is_empty()
            && config.mappings.iter().all(|mapping| {
                crate::gateway_bindings::GATEWAY_BINDINGS
                    .iter()
                    .any(|binding| {
                        binding.service_name == mapping.service
                            && binding.member_name == mapping.member
                            && (binding.instance_name == mapping.instance
                                || binding.instance_specifier == mapping.instance)
                    })
            })
    }

    fn error(context: &str, error: impl Debug) -> MwComProviderError {
        MwComProviderError::Transport(format!("{context}: {error:?}"))
    }

    fn runtime(&self) -> Result<&LolaRuntime, MwComProviderError> {
        self.runtime
            .as_ref()
            .ok_or_else(|| MwComProviderError::Transport("gateway adapter is disconnected".into()))
    }

    fn instance(binding: ScoreBinding<'_>) -> Result<InstanceSpecifier, MwComProviderError> {
        InstanceSpecifier::new(binding.instance_specifier)
            .map_err(|error| Self::error("gateway instance specifier", error))
    }

    fn validate_manifest(&self) -> Result<(), MwComProviderError> {
        let contents = std::fs::read_to_string(&self.config_path)
            .map_err(|error| MwComProviderError::Config(error.to_string()))?;
        let manifest: serde_json::Value = serde_json::from_str(&contents)
            .map_err(|error| MwComProviderError::Config(error.to_string()))?;
        for expected in [
            "/body_control_module/local_tx",
            "/body_control_module/network_rx",
            "/Vehicle/Service2/Instance",
        ] {
            if !manifest["serviceInstances"]
                .as_array()
                .is_some_and(|instances| {
                    instances
                        .iter()
                        .any(|instance| instance["instanceSpecifier"].as_str() == Some(expected))
                })
            {
                return Err(MwComProviderError::Config(format!(
                    "gateway manifest is missing instance {expected}"
                )));
            }
        }
        Ok(())
    }
}

#[async_trait]
impl ScoreRuntimeAdapter for GatewayScoreRuntimeAdapter {
    async fn connect(&mut self) -> Result<(), MwComProviderError> {
        if self.runtime.is_some() {
            return Ok(());
        }
        self.validate_manifest()?;
        let mut builder = LolaRuntimeBuilderImpl::new();
        builder.load_config(&self.config_path);
        let runtime = builder
            .build()
            .map_err(|error| Self::error("gateway runtime", error))?;
        if std::env::var("KUKSA_HIGH_BEAM_ACTUATION").as_deref() != Ok("0") {
            let instance = InstanceSpecifier::new("/body_control_module/local_tx")
                .map_err(|error| Self::error("gateway Tx instance", error))?;
            self.tx = Some(
                runtime
                    .producer_builder::<HighBeamTxInterface>(instance)
                    .build()
                    .map_err(|error| Self::error("gateway Tx build", error))?
                    .offer()
                    .map_err(|error| Self::error("gateway Tx offer", error))?,
            );
        }
        self.runtime = Some(runtime);
        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), MwComProviderError> {
        for subscription in self.subscriptions.drain(..) {
            subscription.unsubscribe();
        }
        if let Some(tx) = self.tx.take() {
            tx.unoffer()
                .map_err(|error| Self::error("gateway Tx unoffer", error))?;
        }
        self.runtime = None;
        self.cursor = 0;
        Ok(())
    }

    async fn find_service(&self, binding: ScoreBinding<'_>) -> Result<bool, MwComProviderError> {
        match binding.service_name {
            "/body_control_module_rx" => Ok(!self
                .runtime()?
                .find_service::<VehicleHighBeamRxInterface>(FindServiceSpecifier::Specific(
                    Self::instance(binding)?,
                ))
                .get_available_instances()
                .map_err(|error| Self::error("gateway body discovery", error))?
                .is_empty()),
            "VehicleDynamicsService" => Ok(!self
                .runtime()?
                .find_service::<VehicleDynamicsServiceInterface>(FindServiceSpecifier::Specific(
                    Self::instance(binding)?,
                ))
                .get_available_instances()
                .map_err(|error| Self::error("gateway speed discovery", error))?
                .is_empty()),
            "/body_control_module_tx" => Ok(self.tx.is_some()),
            _ => Err(MwComProviderError::Transport(
                "unsupported gateway binding".into(),
            )),
        }
    }

    async fn subscribe(&mut self, binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {
        if self
            .subscriptions
            .iter()
            .any(|entry| entry.matches(binding))
        {
            return Ok(());
        }
        match binding.service_name {
            "/body_control_module_rx" => self.subscribe_body(binding),
            "VehicleDynamicsService" if binding.member_name == "speed" => {
                let consumer = self
                    .runtime()?
                    .find_service::<VehicleDynamicsServiceInterface>(
                        FindServiceSpecifier::Specific(Self::instance(binding)?),
                    )
                    .get_available_instances()
                    .map_err(|error| Self::error("speed discovery", error))?
                    .into_iter()
                    .next()
                    .ok_or_else(|| {
                        MwComProviderError::Transport("gateway speed is unavailable".into())
                    })?
                    .build()
                    .map_err(|error| Self::error("speed consumer", error))?;
                let subscription = consumer
                    .speed
                    .subscribe(4)
                    .map_err(|error| Self::error("speed subscribe", error))?;
                self.subscriptions.push(SubscriptionEntry::new(
                    binding,
                    GatewaySubscription::Speed(subscription),
                ));
                Ok(())
            }
            _ => Err(MwComProviderError::Transport(
                "unsupported gateway Rx event".into(),
            )),
        }
    }

    async fn unsubscribe(&mut self, binding: ScoreBinding<'_>) -> Result<(), MwComProviderError> {
        if let Some(index) = self
            .subscriptions
            .iter()
            .position(|entry| entry.matches(binding))
        {
            self.subscriptions.remove(index).unsubscribe();
        }
        Ok(())
    }

    async fn recv(&mut self) -> Result<Option<MwComMessage>, MwComProviderError> {
        let count = self.subscriptions.len();
        for _ in 0..count {
            let index = self.cursor % count;
            self.cursor = (self.cursor + 1) % count;
            if let Some(message) = self.subscriptions[index].poll()? {
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
        if binding.service_name != "/body_control_module_tx" {
            return Err(MwComProviderError::Transport(
                "gateway actuation must use the Tx binding".into(),
            ));
        }
        self.send_body(binding, &message.value)
    }
}

include!("../generated/gateway_runtime.rs");
