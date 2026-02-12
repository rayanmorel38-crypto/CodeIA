// benches/engine_benches.rs

use criterion::{criterion_group, criterion_main, Criterion};
use crate::modules::{reasoning::Reasoning, self_learning::SelfLearning};
use std::time::Duration;
use std::path::Path;
use async_std::task; // utilisé pour lancer des tâches async_std
use tokio::runtime::Runtime; // utilisé pour lancer des tâches tokio futures

/// Benchmark synchrone de l'analyse d'un dossier
fn bench_reasoning_sync(c: &mut Criterion) {
    let dir = Path::new("../resources/sample_project/");
    c.bench_function("Reasoning - analyse_disk sync", |b| {
        b.iter(|| Reasoning::analyze_disk(dir))
    });
}

/// Benchmark asynchrone avec async_std
fn bench_reasoning_async_std(c: &mut Criterion) {
    let dir = Path::new("../resources/sample_project/");
    c.bench_function("Reasoning - analyse_disk async_std", |b| {
        b.iter(|| {
            task::block_on(async {
                task::spawn_blocking(move || {
                    Reasoning::analyze_disk(dir);
                })
                .await; // pas de unwrap, renvoie ()
            });
        })
    });
}

/// Benchmark asynchrone avec tokio
fn bench_reasoning_async_tokio(c: &mut Criterion) {
    let dir = Path::new("../resources/sample_project/");
    let rt = Runtime::new().unwrap();
    c.bench_function("Reasoning - analyse_disk tokio", |b| {
        b.iter(|| {
            rt.block_on(async {
                tokio::task::spawn_blocking(move || {
                    Reasoning::analyze_disk(dir);
                })
                .await
                .ok(); // pas de unwrap, renvoie Result<(), JoinError>
            })
        })
    });
}

/// Benchmark SelfLearning (mise à jour des modèles)
fn bench_self_learning(c: &mut Criterion) {
    c.bench_function("SelfLearning - update_models", |b| {
        b.iter(|| {
            let _ = SelfLearning::update_models();
        })
    });
}

criterion_group! {
    name = benches;
    config = Criterion::default()
        .measurement_time(Duration::from_secs(5))
        .sample_size(30);
    targets = bench_reasoning_sync, bench_reasoning_async_std, bench_reasoning_async_tokio, bench_self_learning
}
criterion_main!(benches);

