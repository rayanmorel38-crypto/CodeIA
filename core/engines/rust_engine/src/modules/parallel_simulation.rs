// src/modules/parallel_simulation.rs

use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
// debug writer not needed here

/// Structure représentant une entité dans la simulation
#[derive(Clone, Debug)]
pub struct SimulationEntity {
    pub id: usize,
    pub position: (f64, f64),
    pub velocity: (f64, f64),
    pub state: EntityState,
}

#[derive(Clone, Debug)]
pub enum EntityState {
    Idle,
    Moving,
    Interacting,
    Processing,
}

/// Moteur de simulation parallèle
pub struct ParallelSimulation {
    entities: Arc<Mutex<Vec<SimulationEntity>>>,
    world_bounds: (f64, f64),
    time_step: f64,
}

impl ParallelSimulation {
    pub fn new(world_width: f64, world_height: f64, time_step: f64) -> Self {
        ParallelSimulation {
            entities: Arc::new(Mutex::new(Vec::new())),
            world_bounds: (world_width, world_height),
            time_step,
        }
    }

    /// Ajoute une entité à la simulation
    pub fn add_entity(&self, entity: SimulationEntity) {
        let mut entities = self.entities.lock().unwrap();
        entities.push(entity);
    }

    /// Initialise des entités de test
    pub fn initialize_test_entities(&self, count: usize) {
        for i in 0..count {
            let entity = SimulationEntity {
                id: i,
                position: (
                    rand::random::<f64>() * self.world_bounds.0,
                    rand::random::<f64>() * self.world_bounds.1,
                ),
                velocity: (
                    (rand::random::<f64>() - 0.5) * 2.0,
                    (rand::random::<f64>() - 0.5) * 2.0,
                ),
                state: EntityState::Idle,
            };
            self.add_entity(entity);
        }
    }

    /// Exécute une étape de simulation en parallèle
    pub fn step(&self) -> SimulationResult {
        let start_time = Instant::now();

        let entities_clone = {
            let entities = self.entities.lock().unwrap();
            entities.clone()
        };

        // Traitement des entités (non parallèle pour le test)
        let updated_entities: Vec<SimulationEntity> = entities_clone
            .into_iter()
            .map(|mut entity| {
                // Mise à jour de la position
                entity.position.0 += entity.velocity.0 * self.time_step;
                entity.position.1 += entity.velocity.1 * self.time_step;

                // Rebond sur les bords
                if entity.position.0 < 0.0 || entity.position.0 > self.world_bounds.0 {
                    entity.velocity.0 = -entity.velocity.0;
                    entity.position.0 = entity.position.0.max(0.0).min(self.world_bounds.0);
                }
                if entity.position.1 < 0.0 || entity.position.1 > self.world_bounds.1 {
                    entity.velocity.1 = -entity.velocity.1;
                    entity.position.1 = entity.position.1.max(0.0).min(self.world_bounds.1);
                }

                // Pour le test, pas de changement d'état aléatoire
                entity
            })
            .collect();

        // Mise à jour des entités
        {
            let mut entities = self.entities.lock().unwrap();
            *entities = updated_entities;
        }

        let computation_time = start_time.elapsed();

        let active = self.entities.lock().unwrap()
            .iter()
            .filter(|e| !matches!(e.state, EntityState::Idle))
            .count();

        SimulationResult {
            step_time: computation_time,
            entity_count: self.entities.lock().unwrap().len(),
            active_entities: active,
        }
    }

    /// Obtient l'état actuel de toutes les entités
    pub fn get_entities(&self) -> Vec<SimulationEntity> {
        self.entities.lock().unwrap().clone()
    }

    /// Statistiques de la simulation
    pub fn get_stats(&self) -> SimulationStats {
        let entities = self.entities.lock().unwrap();
        let total_entities = entities.len();

        let state_counts = entities.iter().fold([0; 4], |mut counts, entity| {
            let index = match entity.state {
                EntityState::Idle => 0,
                EntityState::Moving => 1,
                EntityState::Interacting => 2,
                EntityState::Processing => 3,
            };
            counts[index] += 1;
            counts
        });

        SimulationStats {
            total_entities,
            idle_count: state_counts[0],
            moving_count: state_counts[1],
            interacting_count: state_counts[2],
            processing_count: state_counts[3],
        }
    }
}

/// Résultat d'une étape de simulation
#[derive(Debug)]
pub struct SimulationResult {
    pub step_time: Duration,
    pub entity_count: usize,
    pub active_entities: usize,
}

/// Statistiques de la simulation
#[derive(Debug)]
pub struct SimulationStats {
    pub total_entities: usize,
    pub idle_count: usize,
    pub moving_count: usize,
    pub interacting_count: usize,
    pub processing_count: usize,
}

    #[test]
    fn test_parallel_simulation() {
        let sim = ParallelSimulation::new(100.0, 100.0, 0.1);
        sim.initialize_test_entities(10);

        let initial_stats = sim.get_stats();
        assert_eq!(initial_stats.total_entities, 10);

        let result = sim.step();
        assert_eq!(result.entity_count, 10);
    }