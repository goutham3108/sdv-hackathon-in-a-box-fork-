use databroker::broker::{ActuationChange, DataValue};
use mw_com_provider::score_bindings::{find_score_binding, validate_score_bindings};
use mw_com_provider::{Direction, MwComMapper, MwComValue, ProviderConfig, VssDataType};
use std::path::Path;

fn provider_config() -> ProviderConfig {
    ProviderConfig::load_json(
        Path::new(env!("CARGO_MANIFEST_DIR")).join("generated/mw_com_provider_config.json"),
    )
    .unwrap()
}

#[test]
fn body_control_mapping_matches_wire_and_instance_contract() {
    let config = provider_config();
    config.validate().unwrap();
    validate_score_bindings(&config).unwrap();
    assert_eq!(config.mappings.len(), 39);

    let mut datapoints = 0;
    let mut actuators = 0;
    for mapping in &config.mappings {
        let expected_instance = match mapping.direction {
            Direction::Datapoint => {
                datapoints += 1;
                "standard_vehicle_datapoints"
            }
            Direction::Actuator => {
                actuators += 1;
                "standard_vehicle_actuators"
            }
            Direction::Bidirectional => panic!("BCM mappings must use one direction"),
        };
        assert_eq!(mapping.service, "BodyControlModuleService");
        assert_eq!(mapping.instance, expected_instance);
        assert_eq!(mapping.field.as_deref(), Some("value"));

        let binding =
            find_score_binding(&mapping.service, &mapping.instance, &mapping.member).unwrap();
        assert_eq!(binding.instance_specifier, format!("/{expected_instance}"));
        let (payload_type, sample_size, sample_alignment) = match mapping.datatype {
            VssDataType::Bool => (pascal_type(&mapping.member), 1, 1),
            VssDataType::Uint8 => (pascal_type(&mapping.member), 1, 1),
            VssDataType::String => (pascal_type(&mapping.member), 16, 1),
            VssDataType::Float => (pascal_type(&mapping.member), 4, 4),
            other => panic!("unexpected BCM datatype: {other:?}"),
        };
        assert_eq!(binding.payload_type, payload_type);
        assert_eq!(binding.sample_size, sample_size);
        assert_eq!(binding.sample_alignment, sample_alignment);
    }

    assert_eq!(datapoints, 5);
    assert_eq!(actuators, 34);
}

#[test]
fn body_control_manifest_exposes_both_instances_with_all_events() {
    let manifest_path =
        Path::new(env!("CARGO_MANIFEST_DIR")).join("generated/bcm_config_lola_config.json");
    let manifest: serde_json::Value =
        serde_json::from_str(&std::fs::read_to_string(manifest_path).unwrap()).unwrap();
    let instances = manifest["serviceInstances"].as_array().unwrap();
    assert_eq!(instances.len(), 2);

    for expected in [
        "/standard_vehicle_datapoints",
        "/standard_vehicle_actuators",
    ] {
        let instance = instances
            .iter()
            .find(|instance| instance["instanceSpecifier"] == expected)
            .unwrap();
        assert_eq!(
            instance["serviceTypeName"],
            "vehicle::BodyControlModuleService"
        );
        assert_eq!(
            instance["instances"][0]["events"].as_array().unwrap().len(),
            39
        );
    }
}

fn pascal_type(member: &str) -> String {
    member
        .split('_')
        .map(|part| {
            let mut chars = part.chars();
            chars
                .next()
                .map(|first| first.to_uppercase().collect::<String>() + chars.as_str())
                .unwrap_or_default()
        })
        .collect()
}

#[test]
fn front_wiper_mode_actuation_preserves_string_value() {
    let config = provider_config();
    let signal_id = config
        .mappings
        .iter()
        .find(|mapping| mapping.member == "front_wiper_mode")
        .unwrap()
        .signal_id;
    let mapper = MwComMapper::new(config).unwrap();
    let message = mapper
        .actuation_to_message(&ActuationChange {
            id: signal_id,
            data_value: DataValue::String("INTERMITTENT".into()),
        })
        .unwrap();

    assert_eq!(message.service, "BodyControlModuleService");
    assert_eq!(message.instance, "standard_vehicle_actuators");
    assert_eq!(message.member, "front_wiper_mode");
    assert_eq!(message.value, MwComValue::String("INTERMITTENT".into()));
}
