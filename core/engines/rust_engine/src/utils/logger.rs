// src/logger.rs
use std::fs::{File, OpenOptions};
use std::io::Write;
use std::sync::Mutex;
use chrono::Local;

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
}

impl std::fmt::Display for LogLevel {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            LogLevel::Trace => write!(f, "TRACE"),
            LogLevel::Debug => write!(f, "DEBUG"),
            LogLevel::Info => write!(f, "INFO"),
            LogLevel::Warn => write!(f, "WARN"),
            LogLevel::Error => write!(f, "ERROR"),
        }
    }
}

pub struct Logger {
    file: Mutex<Option<File>>,
    min_level: LogLevel,
    console_enabled: bool,
}

impl Logger {
    pub fn new(log_file: Option<&str>, min_level: LogLevel, console_enabled: bool) -> std::io::Result<Self> {
        let file = if let Some(path) = log_file {
            Some(OpenOptions::new()
                .create(true)
                .append(true)
                .open(path)?)
        } else {
            None
        };

        Ok(Logger {
            file: Mutex::new(file),
            min_level,
            console_enabled,
        })
    }

    pub fn log(&self, level: LogLevel, msg: &str) {
        if level < self.min_level {
            return;
        }

        let timestamp = Local::now().format("%Y-%m-%d %H:%M:%S%.3f");
        let thread_id = std::thread::current().id();
        let formatted = format!("[{}] [{}] [{:?}] {}", timestamp, level, thread_id, msg);

        if self.console_enabled {
            match level {
                LogLevel::Error => eprintln!("{}", formatted),
                _ => println!("{}", formatted),
            }
        }

        if let Ok(mut file_opt) = self.file.lock() {
            if let Some(ref mut file) = *file_opt {
                let _ = writeln!(file, "{}", formatted);
            }
        }
    }

    pub fn trace(&self, msg: &str) { self.log(LogLevel::Trace, msg); }
    pub fn debug(&self, msg: &str) { self.log(LogLevel::Debug, msg); }
    pub fn info(&self, msg: &str) { self.log(LogLevel::Info, msg); }
    pub fn warn(&self, msg: &str) { self.log(LogLevel::Warn, msg); }
    pub fn error(&self, msg: &str) { self.log(LogLevel::Error, msg); }
}

// Global logger instance (lazy_static or once_cell pattern would be better in production)
lazy_static::lazy_static! {
    pub static ref GLOBAL_LOGGER: Logger = {
        Logger::new(Some("logs/rust_engine.log"), LogLevel::Info, true)
            .unwrap_or_else(|_| Logger::new(None, LogLevel::Info, true).unwrap())
    };
}

#[macro_export]
macro_rules! log_trace {
    ($($arg:tt)*) => {
        $crate::logger::GLOBAL_LOGGER.trace(&format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_debug {
    ($($arg:tt)*) => {
        $crate::logger::GLOBAL_LOGGER.debug(&format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_info {
    ($($arg:tt)*) => {
        $crate::logger::GLOBAL_LOGGER.info(&format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_warn {
    ($($arg:tt)*) => {
        $crate::logger::GLOBAL_LOGGER.warn(&format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_error {
    ($($arg:tt)*) => {
        $crate::logger::GLOBAL_LOGGER.error(&format!($($arg)*))
    };
}
