use std::collections::{HashMap, HashSet};
use std::pin::Pin;

use databroker_proto::sdv::databroker::v1 as proto;
use tokio_stream::{Stream, StreamExt};
use tonic::{Request, Response, Status};

use crate::broker::{self, ActuationError, DataBroker, ReadError};
use crate::permissions::Permissions;
use crate::types::{ChangeType, DataType, DataValue};

fn permissions<T>(request: &Request<T>) -> Result<Permissions, Status> {
    request
        .extensions()
        .get::<Permissions>()
        .cloned()
        .ok_or_else(|| Status::unauthenticated("Unauthenticated"))
}

fn failure(reason: proto::datapoint::Failure) -> proto::Datapoint {
    proto::Datapoint {
        timestamp: None,
        value: Some(proto::datapoint::Value::FailureValue(reason as i32)),
    }
}

fn read_failure(error: ReadError) -> proto::Datapoint {
    failure(match error {
        ReadError::NotFound => proto::datapoint::Failure::UnknownDatapoint,
        ReadError::PermissionDenied | ReadError::PermissionExpired => {
            proto::datapoint::Failure::AccessDenied
        }
    })
}

fn from_value(value: DataValue) -> proto::datapoint::Value {
    use proto::datapoint::Value;
    match value {
        DataValue::NotAvailable => {
            Value::FailureValue(proto::datapoint::Failure::NotAvailable as i32)
        }
        DataValue::String(value) => Value::StringValue(value),
        DataValue::Bool(value) => Value::BoolValue(value),
        DataValue::Int32(value) => Value::Int32Value(value),
        DataValue::Int64(value) => Value::Int64Value(value),
        DataValue::Uint32(value) => Value::Uint32Value(value),
        DataValue::Uint64(value) => Value::Uint64Value(value),
        DataValue::Float(value) => Value::FloatValue(value),
        DataValue::Double(value) => Value::DoubleValue(value),
        DataValue::StringArray(values) => Value::StringArray(proto::StringArray { values }),
        DataValue::BoolArray(values) => Value::BoolArray(proto::BoolArray { values }),
        DataValue::Int32Array(values) => Value::Int32Array(proto::Int32Array { values }),
        DataValue::Int64Array(values) => Value::Int64Array(proto::Int64Array { values }),
        DataValue::Uint32Array(values) => Value::Uint32Array(proto::Uint32Array { values }),
        DataValue::Uint64Array(values) => Value::Uint64Array(proto::Uint64Array { values }),
        DataValue::FloatArray(values) => Value::FloatArray(proto::FloatArray { values }),
        DataValue::DoubleArray(values) => Value::DoubleArray(proto::DoubleArray { values }),
    }
}

fn to_value(datapoint: proto::Datapoint) -> Result<DataValue, proto::DatapointError> {
    use proto::datapoint::Value;
    Ok(match datapoint.value {
        Some(Value::StringValue(value)) => DataValue::String(value),
        Some(Value::BoolValue(value)) => DataValue::Bool(value),
        Some(Value::Int32Value(value)) => DataValue::Int32(value),
        Some(Value::Int64Value(value)) => DataValue::Int64(value),
        Some(Value::Uint32Value(value)) => DataValue::Uint32(value),
        Some(Value::Uint64Value(value)) => DataValue::Uint64(value),
        Some(Value::FloatValue(value)) => DataValue::Float(value),
        Some(Value::DoubleValue(value)) => DataValue::Double(value),
        Some(Value::StringArray(array)) => DataValue::StringArray(array.values),
        Some(Value::BoolArray(array)) => DataValue::BoolArray(array.values),
        Some(Value::Int32Array(array)) => DataValue::Int32Array(array.values),
        Some(Value::Int64Array(array)) => DataValue::Int64Array(array.values),
        Some(Value::Uint32Array(array)) => DataValue::Uint32Array(array.values),
        Some(Value::Uint64Array(array)) => DataValue::Uint64Array(array.values),
        Some(Value::FloatArray(array)) => DataValue::FloatArray(array.values),
        Some(Value::DoubleArray(array)) => DataValue::DoubleArray(array.values),
        Some(Value::FailureValue(_)) | None => return Err(proto::DatapointError::InvalidType),
    })
}

fn datapoint(datapoint: broker::Datapoint) -> proto::Datapoint {
    proto::Datapoint {
        timestamp: Some(datapoint.ts.into()),
        value: Some(from_value(datapoint.value)),
    }
}

fn actuation_error(error: ActuationError) -> proto::DatapointError {
    match error {
        ActuationError::NotFound => proto::DatapointError::UnknownDatapoint,
        ActuationError::WrongType | ActuationError::UnsupportedType => {
            proto::DatapointError::InvalidType
        }
        ActuationError::OutOfBounds => proto::DatapointError::OutOfBounds,
        ActuationError::PermissionDenied | ActuationError::PermissionExpired => {
            proto::DatapointError::AccessDenied
        }
        ActuationError::ProviderNotAvailable
        | ActuationError::ProviderAlreadyExists
        | ActuationError::TransmissionFailure => proto::DatapointError::InternalError,
    }
}

fn metadata(metadata: &broker::Metadata) -> proto::Metadata {
    let data_type = match metadata.data_type {
        DataType::String => proto::DataType::String,
        DataType::Bool => proto::DataType::Bool,
        DataType::Int8 => proto::DataType::Int8,
        DataType::Int16 => proto::DataType::Int16,
        DataType::Int32 => proto::DataType::Int32,
        DataType::Int64 => proto::DataType::Int64,
        DataType::Uint8 => proto::DataType::Uint8,
        DataType::Uint16 => proto::DataType::Uint16,
        DataType::Uint32 => proto::DataType::Uint32,
        DataType::Uint64 => proto::DataType::Uint64,
        DataType::Float => proto::DataType::Float,
        DataType::Double => proto::DataType::Double,
        DataType::StringArray => proto::DataType::StringArray,
        DataType::BoolArray => proto::DataType::BoolArray,
        DataType::Int8Array => proto::DataType::Int8Array,
        DataType::Int16Array => proto::DataType::Int16Array,
        DataType::Int32Array => proto::DataType::Int32Array,
        DataType::Int64Array => proto::DataType::Int64Array,
        DataType::Uint8Array => proto::DataType::Uint8Array,
        DataType::Uint16Array => proto::DataType::Uint16Array,
        DataType::Uint32Array => proto::DataType::Uint32Array,
        DataType::Uint64Array => proto::DataType::Uint64Array,
        DataType::FloatArray => proto::DataType::FloatArray,
        DataType::DoubleArray => proto::DataType::DoubleArray,
    };
    proto::Metadata {
        id: metadata.id,
        name: metadata.path.clone(),
        data_type: data_type as i32,
        change_type: match metadata.change_type {
            ChangeType::Static => proto::ChangeType::Static,
            ChangeType::OnChange => proto::ChangeType::OnChange,
            ChangeType::Continuous => proto::ChangeType::Continuous,
        } as i32,
        description: metadata.description.clone(),
    }
}

fn subscription_paths(query: &str) -> Result<Vec<String>, Status> {
    let query = query.trim();
    let (keyword, paths) = query
        .split_once(char::is_whitespace)
        .ok_or_else(|| Status::invalid_argument("Expected SELECT path[, path...]"))?;
    if !keyword.eq_ignore_ascii_case("SELECT") {
        return Err(Status::invalid_argument("Expected SELECT path[, path...]"));
    }
    let paths: Vec<String> = paths
        .split(',')
        .map(|path| path.trim().to_owned())
        .collect();
    if paths.iter().any(|path| {
        path.split('.').any(|segment| {
            segment.is_empty()
                || !segment
                    .chars()
                    .all(|character| character.is_ascii_alphanumeric() || character == '_')
        })
    }) {
        return Err(Status::invalid_argument("Only SELECT with explicit VSS paths is supported; expressions and WHERE clauses are not supported"));
    }
    Ok(paths)
}

#[tonic::async_trait]
impl proto::broker_server::Broker for DataBroker {
    async fn get_datapoints(
        &self,
        request: Request<proto::GetDatapointsRequest>,
    ) -> Result<Response<proto::GetDatapointsReply>, Status> {
        let permissions = permissions(&request)?;
        let access = self.authorized_access(&permissions);
        let paths = request.into_inner().datapoints;
        if paths.is_empty() {
            return Err(Status::invalid_argument("No datapoints requested"));
        }
        let mut datapoints = HashMap::new();
        for path in paths {
            let value = match access.get_datapoint_by_path(&path).await {
                Ok(value) => datapoint(value),
                Err(error) => read_failure(error),
            };
            datapoints.insert(path, value);
        }
        Ok(Response::new(proto::GetDatapointsReply { datapoints }))
    }

    async fn set_datapoints(
        &self,
        request: Request<proto::SetDatapointsRequest>,
    ) -> Result<Response<proto::SetDatapointsReply>, Status> {
        let permissions = permissions(&request)?;
        let access = self.authorized_access(&permissions);
        let updates = request.into_inner().datapoints;
        if updates.is_empty() {
            return Err(Status::invalid_argument("No datapoints provided"));
        }
        let mut errors = HashMap::new();
        for (path, datapoint) in updates {
            let Some(id) = access.get_id_by_path(&path).await else {
                errors.insert(path, proto::DatapointError::UnknownDatapoint as i32);
                continue;
            };
            let value = match to_value(datapoint) {
                Ok(value) => value,
                Err(error) => {
                    errors.insert(path, error as i32);
                    continue;
                }
            };
            if let Err((error, _)) = access.actuate(&id, &value).await {
                errors.insert(path, actuation_error(error) as i32);
            }
        }
        Ok(Response::new(proto::SetDatapointsReply { errors }))
    }

    async fn get_metadata(
        &self,
        request: Request<proto::GetMetadataRequest>,
    ) -> Result<Response<proto::GetMetadataReply>, Status> {
        let permissions = permissions(&request)?;
        let access = self.authorized_access(&permissions);
        let names: HashSet<_> = request.into_inner().names.into_iter().collect();
        let list = access
            .filter_map_entries(|entry| {
                let entry_metadata = entry.metadata();
                if names.is_empty() || names.contains(&entry_metadata.path) {
                    Some(metadata(entry_metadata))
                } else {
                    None
                }
            })
            .await;
        Ok(Response::new(proto::GetMetadataReply { list }))
    }

    type SubscribeStream =
        Pin<Box<dyn Stream<Item = Result<proto::SubscribeReply, Status>> + Send>>;

    async fn subscribe(
        &self,
        request: Request<proto::SubscribeRequest>,
    ) -> Result<Response<Self::SubscribeStream>, Status> {
        let permissions = permissions(&request)?;
        let access = self.authorized_access(&permissions);
        let paths = subscription_paths(&request.into_inner().query)?;
        let mut entries = HashMap::new();
        let mut names = HashMap::new();
        for path in paths {
            let entry = access
                .get_entry_by_path(&path)
                .await
                .map_err(|error| match error {
                    ReadError::NotFound => Status::not_found(format!("Unknown datapoint: {path}")),
                    ReadError::PermissionDenied | ReadError::PermissionExpired => {
                        Status::permission_denied("Read access denied")
                    }
                })?;
            entries.insert(entry.metadata.id, HashSet::from([broker::Field::Datapoint]));
            names.insert(entry.metadata.id, path);
        }
        let stream = access
            .subscribe(entries, Some(100), None)
            .await
            .map_err(|error| Status::invalid_argument(format!("Subscription failed: {error:?}")))?;
        let output = async_stream::try_stream! {
            tokio::pin!(stream);
            while let Some(notification) = stream.next().await {
                let notification = notification.ok_or_else(|| Status::data_loss("Subscription updates lost"))?;
                let mut fields = HashMap::new();
                for update in notification.updates {
                    if let (Some(name), Some(value)) = (names.get(&update.id), update.update.datapoint) {
                        fields.insert(name.clone(), datapoint(value));
                    }
                }
                if !fields.is_empty() {
                    yield proto::SubscribeReply { fields };
                }
            }
        };
        Ok(Response::new(Box::pin(output)))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::broker::{ActuationChange, ActuationProvider, EntryType, EntryUpdate};
    use crate::permissions::{ALLOW_ALL, ALLOW_NONE};
    use proto::broker_server::Broker;
    use std::sync::{Arc, Mutex};
    use std::time::{Duration, SystemTime};

    const LIGHT: &str = "Vehicle.Body.Lights.Beam.Low.IsOn";

    struct Provider(Arc<Mutex<Vec<ActuationChange>>>);

    #[async_trait::async_trait]
    impl ActuationProvider for Provider {
        async fn actuate(
            &self,
            changes: Vec<ActuationChange>,
        ) -> Result<(), (ActuationError, String)> {
            self.0.lock().unwrap().extend(changes);
            Ok(())
        }

        fn is_available(&self) -> bool {
            true
        }
    }

    fn request<T>(message: T) -> Request<T> {
        let mut request = Request::new(message);
        request.extensions_mut().insert(ALLOW_ALL.clone());
        request
    }

    async fn seed(broker: &DataBroker) -> i32 {
        broker
            .authorized_access(&ALLOW_ALL)
            .add_entry(
                LIGHT.to_owned(),
                DataType::Bool,
                ChangeType::OnChange,
                EntryType::Actuator,
                "Low beam".to_owned(),
                None,
                None,
                None,
                None,
            )
            .await
            .unwrap()
    }

    async fn publish(broker: &DataBroker, id: i32, value: bool) {
        broker
            .authorized_access(&ALLOW_ALL)
            .update_entries([(
                id,
                EntryUpdate {
                    datapoint: Some(broker::Datapoint {
                        ts: SystemTime::now(),
                        source_ts: None,
                        value: DataValue::Bool(value),
                    }),
                    ..Default::default()
                },
            )])
            .await
            .unwrap();
    }

    async fn read(broker: &DataBroker) -> proto::Datapoint {
        Broker::get_datapoints(
            broker,
            request(proto::GetDatapointsRequest {
                datapoints: vec![LIGHT.to_owned()],
            }),
        )
        .await
        .unwrap()
        .into_inner()
        .datapoints
        .remove(LIGHT)
        .unwrap()
    }

    #[test]
    fn value_roundtrips_and_invalid_values() {
        let values = [
            DataValue::String("test".into()),
            DataValue::Bool(true),
            DataValue::Int32(-42),
            DataValue::Int64(i64::MIN),
            DataValue::Uint32(u32::MAX),
            DataValue::Uint64(u64::MAX),
            DataValue::Float(1.25),
            DataValue::Double(2.5),
            DataValue::StringArray(vec!["test".into()]),
            DataValue::BoolArray(vec![true, false]),
            DataValue::Int32Array(vec![-42]),
            DataValue::Int64Array(vec![i64::MIN]),
            DataValue::Uint32Array(vec![u32::MAX]),
            DataValue::Uint64Array(vec![u64::MAX]),
            DataValue::FloatArray(vec![1.25]),
            DataValue::DoubleArray(vec![2.5]),
        ];
        for value in values {
            let encoded = proto::Datapoint {
                timestamp: None,
                value: Some(from_value(value.clone())),
            };
            assert_eq!(to_value(encoded).unwrap(), value);
        }
        assert_eq!(
            to_value(proto::Datapoint::default()).unwrap_err(),
            proto::DatapointError::InvalidType
        );
        assert_eq!(
            to_value(failure(proto::datapoint::Failure::NotAvailable)).unwrap_err(),
            proto::DatapointError::InvalidType
        );
        assert_eq!(
            actuation_error(ActuationError::OutOfBounds),
            proto::DatapointError::OutOfBounds
        );
    }

    #[test]
    fn select_queries_are_explicit_and_never_ignore_conditions() {
        assert_eq!(
            subscription_paths(&format!("select {LIGHT}, Vehicle.Speed")).unwrap(),
            vec![LIGHT, "Vehicle.Speed"]
        );
        for query in [
            "",
            "SELECT",
            "SELECT ",
            "SELECT ,",
            "SELECT *",
            "SELECT Vehicle..Speed",
            "SELECT Vehicle.Speed WHERE Vehicle.Speed > 10",
            "DELETE Vehicle.Speed",
        ] {
            assert!(subscription_paths(query).is_err(), "{query}");
        }
    }

    #[tokio::test]
    async fn writes_reach_provider_and_reads_wait_for_feedback() {
        let broker = DataBroker::default();
        let id = seed(&broker).await;
        publish(&broker, id, false).await;
        let changes = Arc::new(Mutex::new(Vec::new()));
        broker
            .authorized_access(&ALLOW_ALL)
            .provide_actuation(vec![id], Box::new(Provider(changes.clone())))
            .await
            .unwrap();
        let response = Broker::set_datapoints(
            &broker,
            request(proto::SetDatapointsRequest {
                datapoints: HashMap::from([(
                    LIGHT.to_owned(),
                    proto::Datapoint {
                        timestamp: None,
                        value: Some(proto::datapoint::Value::BoolValue(true)),
                    },
                )]),
            }),
        )
        .await
        .unwrap()
        .into_inner();
        assert!(response.errors.is_empty());
        assert_eq!(changes.lock().unwrap()[0].id, id);
        assert_eq!(changes.lock().unwrap()[0].data_value, DataValue::Bool(true));
        assert_eq!(
            read(&broker).await.value,
            Some(proto::datapoint::Value::BoolValue(false))
        );
        publish(&broker, id, true).await;
        assert_eq!(
            read(&broker).await.value,
            Some(proto::datapoint::Value::BoolValue(true))
        );
    }

    #[tokio::test]
    async fn errors_and_permissions_are_preserved() {
        let broker = DataBroker::default();
        let id = seed(&broker).await;
        let mut updates = HashMap::new();
        updates.insert(
            LIGHT.to_owned(),
            proto::Datapoint {
                timestamp: None,
                value: Some(proto::datapoint::Value::StringValue("wrong type".into())),
            },
        );
        updates.insert("Vehicle.Unknown".to_owned(), proto::Datapoint::default());
        let reply = Broker::set_datapoints(
            &broker,
            request(proto::SetDatapointsRequest {
                datapoints: updates,
            }),
        )
        .await
        .unwrap()
        .into_inner();
        assert_eq!(
            reply.errors[LIGHT],
            proto::DatapointError::InvalidType as i32
        );
        assert_eq!(
            reply.errors["Vehicle.Unknown"],
            proto::DatapointError::UnknownDatapoint as i32
        );
        assert_eq!(
            read(&broker).await.value,
            Some(proto::datapoint::Value::FailureValue(
                proto::datapoint::Failure::NotAvailable as i32
            ))
        );
        let mut denied = Request::new(proto::GetDatapointsRequest {
            datapoints: vec![LIGHT.into()],
        });
        denied.extensions_mut().insert(ALLOW_NONE.clone());
        let reply = Broker::get_datapoints(&broker, denied)
            .await
            .unwrap()
            .into_inner();
        assert_eq!(
            reply.datapoints[LIGHT].value,
            Some(proto::datapoint::Value::FailureValue(
                proto::datapoint::Failure::AccessDenied as i32
            ))
        );
        let mut denied = Request::new(proto::SetDatapointsRequest {
            datapoints: HashMap::from([(
                LIGHT.into(),
                proto::Datapoint {
                    timestamp: None,
                    value: Some(proto::datapoint::Value::BoolValue(true)),
                },
            )]),
        });
        denied.extensions_mut().insert(ALLOW_NONE.clone());
        let reply = Broker::set_datapoints(&broker, denied)
            .await
            .unwrap()
            .into_inner();
        assert_eq!(
            reply.errors[LIGHT],
            proto::DatapointError::AccessDenied as i32
        );
        assert_eq!(
            Broker::get_datapoints(
                &broker,
                Request::new(proto::GetDatapointsRequest::default())
            )
            .await
            .unwrap_err()
            .code(),
            tonic::Code::Unauthenticated
        );
        let reply = Broker::set_datapoints(
            &broker,
            request(proto::SetDatapointsRequest {
                datapoints: HashMap::from([(
                    LIGHT.into(),
                    proto::Datapoint {
                        timestamp: None,
                        value: Some(proto::datapoint::Value::BoolValue(true)),
                    },
                )]),
            }),
        )
        .await
        .unwrap()
        .into_inner();
        assert_eq!(
            reply.errors[LIGHT],
            proto::DatapointError::InternalError as i32
        );
        publish(&broker, id, false).await;
    }

    #[tokio::test]
    async fn metadata_and_subscriptions_use_shared_state() {
        let broker = DataBroker::default();
        let id = seed(&broker).await;
        publish(&broker, id, false).await;
        let metadata = Broker::get_metadata(
            &broker,
            request(proto::GetMetadataRequest {
                names: vec![LIGHT.into()],
            }),
        )
        .await
        .unwrap()
        .into_inner();
        assert_eq!(metadata.list.len(), 1);
        assert_eq!(metadata.list[0].data_type, proto::DataType::Bool as i32);
        assert_eq!(
            metadata.list[0].change_type,
            proto::ChangeType::OnChange as i32
        );
        let mut stream = Broker::subscribe(
            &broker,
            request(proto::SubscribeRequest {
                query: format!("SELECT {LIGHT}"),
            }),
        )
        .await
        .unwrap()
        .into_inner();
        let initial = tokio::time::timeout(Duration::from_secs(2), stream.next())
            .await
            .unwrap()
            .unwrap()
            .unwrap();
        assert_eq!(
            initial.fields[LIGHT].value,
            Some(proto::datapoint::Value::BoolValue(false))
        );
        publish(&broker, id, true).await;
        let update = tokio::time::timeout(Duration::from_secs(2), stream.next())
            .await
            .unwrap()
            .unwrap()
            .unwrap();
        assert_eq!(
            update.fields[LIGHT].value,
            Some(proto::datapoint::Value::BoolValue(true))
        );
    }

    #[tokio::test]
    async fn legacy_and_val_services_coexist_over_tcp() {
        use crate::authorization::Authorization;
        use crate::grpc::server::{self, Api};
        use databroker_proto::kuksa::val::v1 as val;
        use tokio_stream::wrappers::TcpListenerStream;

        let broker = DataBroker::default();
        let id = seed(&broker).await;
        publish(&broker, id, true).await;
        let listener = tokio::net::TcpListener::bind("127.0.0.1:0").await.unwrap();
        let address = listener.local_addr().unwrap();
        let (shutdown_tx, shutdown_rx) = tokio::sync::oneshot::channel();
        let task = tokio::spawn(async move {
            server::serve_with_incoming_shutdown(
                TcpListenerStream::new(listener),
                broker,
                #[cfg(feature = "tls")]
                server::ServerTLS::Disabled,
                &[Api::KuksaValV1, Api::KuksaValV2, Api::SdvDatabrokerV1],
                Authorization::Disabled,
                async {
                    let _ = shutdown_rx.await;
                },
            )
            .await
            .map_err(|error| error.to_string())
        });
        let channel = tonic::transport::Endpoint::from_shared(format!("http://{address}"))
            .unwrap()
            .connect()
            .await
            .unwrap();
        let mut legacy = proto::broker_client::BrokerClient::new(channel.clone());
        let reply = legacy
            .get_datapoints(proto::GetDatapointsRequest {
                datapoints: vec![LIGHT.into()],
            })
            .await
            .unwrap()
            .into_inner();
        assert_eq!(
            reply.datapoints[LIGHT].value,
            Some(proto::datapoint::Value::BoolValue(true))
        );
        let mut val_client = val::val_client::ValClient::new(channel);
        let reply = val_client
            .get(val::GetRequest {
                entries: vec![val::EntryRequest {
                    path: LIGHT.into(),
                    view: val::View::CurrentValue as i32,
                    fields: vec![],
                }],
            })
            .await
            .unwrap()
            .into_inner();
        assert!(reply.errors.is_empty());
        assert_eq!(
            reply.entries[0].value.as_ref().unwrap().value,
            Some(val::datapoint::Value::Bool(true))
        );
        drop(legacy);
        drop(val_client);
        shutdown_tx.send(()).unwrap();
        tokio::time::timeout(Duration::from_secs(3), task)
            .await
            .unwrap()
            .unwrap()
            .unwrap();
    }
}
