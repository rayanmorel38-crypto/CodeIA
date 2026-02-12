// tests/integration_tests.rs
use rust_engine::{
    Result,
    core::task_runner::{Task, TaskRunner},
};
use std::time::Duration;

#[tokio::test]
async fn test_task_runner_basic() -> Result<()> {
    let runner = TaskRunner::new(4);
    let task = Task::new("basic_task".to_string(), 1);

    let result = runner
        .run(task, || async { Ok("test completed".to_string()) })
        .await?;

    assert!(result.success);
    assert_eq!(result.message, "test completed");
    Ok(())
}

#[tokio::test]
async fn test_task_runner_timeout() -> Result<()> {
    let runner = TaskRunner::new(4);
    let task = Task::new("timeout_task".to_string(), 1)
        .with_timeout(Duration::from_millis(100));

    let result = runner
        .run(task, || async {
            tokio::time::sleep(Duration::from_secs(1)).await;
            Ok("should not reach".to_string())
        })
        .await?;

    assert!(!result.success); // Task timed out
    assert!(result.message.contains("timeout"));
    Ok(())
}

#[tokio::test]
async fn test_task_runner_error_handling() -> Result<()> {
    let runner = TaskRunner::new(4);
    let task = Task::new("error_task".to_string(), 1);

    let result = runner
        .run(task, || async {
            Err(rust_engine::EngineError::TaskError(
                "intentional error".to_string(),
            ))
        })
        .await?;

    assert!(!result.success);
    assert!(result.message.contains("intentional error"));
    Ok(())
}

#[tokio::test]
async fn test_task_runner_multiple_tasks() -> Result<()> {
    let runner = TaskRunner::new(4);

    for i in 0..5 {
        let task = Task::new(format!("task_{}", i), (i % 3) as u8);
        runner
            .run(task, || async { Ok(format!("task {} done", i)) })
            .await?;
    }

    let results = runner.get_results().await;
    assert_eq!(results.len(), 5);
    assert!(results.iter().all(|r| r.success));

    let summary = runner.get_summary().await;
    assert!(summary.contains("5 succeeded"));
    Ok(())
}
