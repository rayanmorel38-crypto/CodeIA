use warp::Filter;
use crate::core::metrics;

pub async fn serve_metrics() {
    let port = std::env::var("METRICS_PORT")
        .ok()
        .and_then(|s| s.parse::<u16>().ok())
        .unwrap_or(9898);

    let metrics_route = warp::path!("metrics").map(|| {
        let body = metrics::gather_metrics();
        warp::reply::with_header(body, "Content-Type", "text/plain; version=0.0.4")
    });
    warp::serve(metrics_route).run(([0, 0, 0, 0], port)).await;
}
