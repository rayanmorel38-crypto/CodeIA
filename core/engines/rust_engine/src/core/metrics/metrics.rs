use prometheus::{Encoder, TextEncoder, Registry, IntCounterVec, IntGaugeVec, HistogramVec};
use lazy_static::lazy_static;

lazy_static! {
    pub static ref REGISTRY: Registry = Registry::new();
    pub static ref TASK_COUNTER: IntCounterVec = IntCounterVec::new(
        prometheus::Opts::new("tasks_total", "Nombre total de tâches traitées"),
        &["type", "status"]
    ).unwrap();
    pub static ref TASK_DURATION: HistogramVec = HistogramVec::new(
        prometheus::HistogramOpts::new("task_duration_seconds", "Durée des tâches en secondes"),
        &["type"]
    ).unwrap();
    pub static ref ENGINE_GAUGE: IntGaugeVec = IntGaugeVec::new(
        prometheus::Opts::new("engine_state", "État du moteur IA"),
        &["module"]
    ).unwrap();
}

pub fn register_metrics() {
    REGISTRY.register(Box::new(TASK_COUNTER.clone())).ok();
    REGISTRY.register(Box::new(TASK_DURATION.clone())).ok();
    REGISTRY.register(Box::new(ENGINE_GAUGE.clone())).ok();
}

pub fn gather_metrics() -> String {
    let metric_families = REGISTRY.gather();
    let mut buffer = Vec::new();
    let encoder = TextEncoder::new();
    encoder.encode(&metric_families, &mut buffer).unwrap();
    String::from_utf8(buffer).unwrap()
}
