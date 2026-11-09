use databroker::broker::{ActuationChange, DataValue};
use mw_com_provider::gateway_samples::{BodySignalText, HighBeamCommand};
use mw_com_provider::score_bindings::{find_score_binding, validate_score_bindings};
use mw_com_provider::{
    Direction, MwComMapper, MwComMessage, MwComValue, ProviderConfig, SignalQuality, VssDataType,
};
use std::path::Path;
use std::time::SystemTime;

fn config() -> ProviderConfig {
    ProviderConfig::load_json(
        Path::new(env!("CARGO_MANIFEST_DIR"))
            .join("../../sdv-hack-demo/config/gateway_provider_config.json"),
    )
    .unwrap()
}

#[test]
fn every_mapping_has_matching_receive_and_target_metadata() {
    let config = config();
    config.validate().unwrap();
    validate_score_bindings(&config).unwrap();
    assert_eq!(config.mappings.len(), 45);
    for mapping in &config.mappings {
        assert_ne!(mapping.vss_path, "Vehicle.Body.Lights.Beam.Low.IsOn");
        let binding =
            find_score_binding(&mapping.service, &mapping.instance, &mapping.member).unwrap();
        if mapping.vss_path == "Vehicle.Speed" {
            assert_eq!(mapping.direction, Direction::Datapoint);
            assert_eq!(binding.instance_specifier, "/Vehicle/Service2/Instance");
            assert_eq!(binding.sample_size, 16);
            continue;
        }
        if mapping.direction == Direction::Datapoint {
            assert_eq!(binding.instance_specifier, "/body_control_module/network_rx");
            assert!(mapping.actuation_binding.is_none());
            assert_eq!(
                binding.sample_size,
                match mapping.datatype {
                    VssDataType::Bool => 1,
                    VssDataType::Uint8 => 32,
                    VssDataType::String | VssDataType::Int16 | VssDataType::Float
                    | VssDataType::FloatArray => 48,
                    _ => panic!("unsupported sensor datatype"),
                }
            );
            continue;
        }
        if mapping.direction == Direction::Actuator {
            assert_eq!(mapping.service, "/body_control_module_tx");
            assert_eq!(mapping.instance, "local_tx");
            assert!(mapping.actuation_binding.is_none());
            assert_eq!(binding.instance_specifier, "/body_control_module/local_tx");
            assert_eq!(binding.payload_type, "BodySignalText");
            continue;
        }
        assert_eq!(mapping.direction, Direction::Bidirectional);
        assert_eq!(binding.instance_specifier, "/body_control_module/network_rx");
        let target = mapping.actuation_binding.as_ref().unwrap();
        let tx = find_score_binding(&target.service, &target.instance, &target.member).unwrap();
        assert_eq!(tx.instance_specifier, "/body_control_module/local_tx");
        assert_eq!(tx.member_name, binding.member_name);
        assert_eq!(tx.sample_alignment, 16);
        assert_eq!(
            tx.sample_size,
            if mapping.datatype == VssDataType::String {
                48
            } else {
                32
            }
        );
        assert_eq!(
            binding.sample_size,
            match mapping.datatype {
                VssDataType::Bool => 1,
                VssDataType::Uint8 => 32,
                VssDataType::String => 48,
                _ => panic!("unsupported body datatype"),
            }
        );
    }
}

#[test]
fn existing_body_targets_and_current_values_use_distinct_directions() {
    let config = config();
    let mapper = MwComMapper::new(config.clone()).unwrap();
    let mut checked = 0;
    for mapping in &config.mappings {
        if mapping.vss_path == "Vehicle.Speed" {
            continue;
        }
        if mapping.direction == Direction::Datapoint {
            continue;
        }
        let value = match mapping.datatype {
            VssDataType::Bool => DataValue::Bool(true),
            VssDataType::Uint8 => DataValue::Uint32(50),
            VssDataType::String => DataValue::String("FAST".into()),
            _ => panic!("unsupported body datatype"),
        };
        let command = mapper
            .actuation_to_message(&ActuationChange {
                id: mapping.signal_id,
                data_value: value.clone(),
            })
            .unwrap();
        assert_eq!(command.service, "/body_control_module_tx");
        assert_eq!(command.instance, "local_tx");
        if mapping.direction == Direction::Actuator {
            let encoded = BodySignalText::encode(&command.value, "string").unwrap();
            assert_eq!(encoded.decode().unwrap(), MwComValue::String("FAST".into()));
            checked += 1;
            continue;
        }
        let received = match mapping.datatype {
            VssDataType::Bool => {
                let raw = HighBeamCommand::encode(&command.value, "boolean").unwrap();
                assert_eq!(raw.size, 1);
                MwComValue::Bool(raw.data[0] != 0)
            }
            VssDataType::Uint8 => HighBeamCommand::encode(&command.value, "uint8")
                .unwrap()
                .decode()
                .unwrap(),
            VssDataType::String => BodySignalText::encode(&command.value, "string")
                .unwrap()
                .decode()
                .unwrap(),
            _ => unreachable!(),
        };
        let (id, update) = mapper
            .message_to_update(&MwComMessage {
                service: mapping.service.clone(),
                instance: mapping.instance.clone(),
                member: mapping.member.clone(),
                value: received,
                source_timestamp: Some(SystemTime::now()),
                quality: SignalQuality::Valid,
            })
            .unwrap();
        assert_eq!(id, mapping.signal_id);
        assert_eq!(update.datapoint.unwrap().value, value);
        checked += 1;
    }
    assert_eq!(checked, 38);
}
