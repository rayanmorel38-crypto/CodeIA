// src/lib.rs
pub mod core;
pub mod benches;
pub mod engine_modes;
pub mod modules;
pub mod utils;

pub use utils::error::{EngineError, Result};