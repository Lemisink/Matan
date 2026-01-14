use serde::{Deserialize, Serialize};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;
use tauri::Manager;

#[derive(Debug, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct TaskParams {
  pub task: String,
  pub func: String,
  pub a: String,
  pub b: String,
  pub task1_method: String,
  pub eps: String,
  pub h: String,
  pub rmse_sweep: bool,
}

#[derive(Debug, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Task1Data {
  pub func: Vec<[f64; 2]>,
  pub points: Vec<[f64; 3]>,
  pub intervals: Vec<[f64; 4]>,
  pub x_min: f64,
  pub f_min: f64,
}

#[derive(Debug, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Task2Data {
  pub combined: Vec<[f64; 6]>,
  pub rmse: Vec<[f64; 4]>,
}

#[derive(Debug, Serialize)]
pub struct TaskResponse {
  pub task: String,
  pub task1: Option<Task1Data>,
  pub task2: Option<Task2Data>,
}

#[tauri::command]
pub fn run_task(app: tauri::AppHandle, params: TaskParams) -> Result<TaskResponse, String> {
  let run_dir = app
    .path()
    .app_data_dir()
    .map_err(|err| format!("Failed to resolve app data dir: {err}"))?
    .join("matan");
  let data_dir = run_dir.join("data");
  fs::create_dir_all(&data_dir)
    .map_err(|err| format!("Failed to create data directory: {err}"))?;

  let config_path = run_dir.join("config.ini");
  write_config(&config_path, &params, &data_dir)?;

  let matan_path = resolve_matan_binary(&app)?;
  run_matan(&matan_path, &config_path)?;

  match params.task.as_str() {
    "minimize" | "minimization" | "task1" | "1" => {
      let method = params.task1_method.to_lowercase();
      let func = read_dat::<2>(&data_dir.join("task1_func.dat"))?;
      let points = read_dat::<3>(&data_dir.join(format!("task1_{method}_points.dat")))?;
      let intervals = read_dat::<4>(&data_dir.join(format!("task1_{method}_interval.dat")))?;
      let (x_min, f_min) = read_summary(&data_dir.join("task1_summary.dat"))?;
      Ok(TaskResponse {
        task: "minimize".to_string(),
        task1: Some(Task1Data {
          func,
          points,
          intervals,
          x_min,
          f_min,
        }),
        task2: None,
      })
    }
    "differentiate" | "differentiation" | "task2" | "2" | "diff" | "derivative" => {
      let combined = read_dat::<6>(&data_dir.join("task2_all.dat"))?;
      let rmse = if params.rmse_sweep {
        read_dat::<4>(&data_dir.join("task2_rmse.dat"))?
      } else {
        Vec::new()
      };
      Ok(TaskResponse {
        task: "differentiate".to_string(),
        task1: None,
        task2: Some(Task2Data {
          combined,
          rmse,
        }),
      })
    }
    _ => Err("Unknown task value".to_string()),
  }
}

fn resolve_matan_binary(app: &tauri::AppHandle) -> Result<PathBuf, String> {
  if let Ok(path) = std::env::var("MATAN_APP_PATH") {
    let candidate = PathBuf::from(path);
    if candidate.exists() {
      return Ok(candidate);
    }
  }

  let exe_name = if cfg!(target_os = "windows") {
    "matan_app.exe"
  } else {
    "matan_app"
  };

  if let Ok(resource_dir) = app.path().resource_dir() {
    let candidates = [
      resource_dir.join("bin").join(exe_name),
      resource_dir.join("dist").join("bin").join(exe_name),
      resource_dir
        .join("_up_")
        .join("dist")
        .join("bin")
        .join(exe_name),
      resource_dir
        .join("_up_")
        .join("_up_")
        .join("dist")
        .join("bin")
        .join(exe_name),
      resource_dir
        .join("_up_")
        .join("_up_")
        .join("_up_")
        .join("dist")
        .join("bin")
        .join(exe_name),
    ];
    for candidate in candidates {
      if candidate.exists() {
        return Ok(candidate);
      }
    }
  }

  if let Ok(current) = std::env::current_dir() {
    let candidates = [
      current.join("dist").join("bin").join(exe_name),
      current.join("..").join("dist").join("bin").join(exe_name),
      current.join("..").join("..").join("dist").join("bin").join(exe_name),
      current.join("build").join(exe_name),
      current.join("..").join("build").join(exe_name),
      current.join("..").join("..").join("build").join(exe_name),
      current.join("..").join("core").join("build").join(exe_name),
      current.join("..").join("..").join("core").join("build").join(exe_name),
    ];
    for candidate in candidates {
      if candidate.exists() {
        return Ok(candidate);
      }
    }
  }

  Err(format!(
    "matan_app not found. Set MATAN_APP_PATH or place it in dist/bin or resources/bin/{exe_name}"
  ))
}

fn write_config(path: &Path, params: &TaskParams, data_dir: &Path) -> Result<(), String> {
  let contents = format!(
"[general]\n\
task = {task}\n\
func = {func}\n\
a = {a}\n\
b = {b}\n\
\n\
[task1]\n\
method = {task1_method}\n\
eps = {eps}\n\
\n\
[task2]\n\
h = {h}\n\
rmse_sweep = {rmse_sweep}\n\
\n\
[output]\n\
data_dir = {data_dir}\n",
    task = params.task,
    func = params.func,
    a = params.a,
    b = params.b,
    task1_method = params.task1_method,
    eps = params.eps,
    h = params.h,
    rmse_sweep = params.rmse_sweep,
    data_dir = data_dir.display()
  );
  fs::write(path, contents).map_err(|err| format!("Failed to write config: {err}"))
}

fn run_matan(binary: &Path, config_path: &Path) -> Result<(), String> {
  let output = Command::new(binary)
    .arg(config_path)
    .output()
    .map_err(|err| format!("Failed to run matan_app: {err}"))?;
  if output.status.success() {
    return Ok(());
  }
  let stdout = String::from_utf8_lossy(&output.stdout);
  let stderr = String::from_utf8_lossy(&output.stderr);
  Err(format!(
    "matan_app failed (code {:?}). stdout: {stdout} stderr: {stderr}",
    output.status.code()
  ))
}

fn read_dat<const N: usize>(path: &Path) -> Result<Vec<[f64; N]>, String> {
  let contents =
    fs::read_to_string(path).map_err(|err| format!("Failed to read {}: {err}", path.display()))?;
  let mut rows = Vec::new();
  for (line_idx, line) in contents.lines().enumerate() {
    let line = line.trim();
    if line.is_empty() {
      continue;
    }
    let mut values = [0.0f64; N];
    let mut iter = line.split_whitespace();
    for idx in 0..N {
      let token = iter
        .next()
        .ok_or_else(|| format!("Too few columns in {} line {}", path.display(), line_idx + 1))?;
      values[idx] = token.parse::<f64>().map_err(|err| {
        format!(
          "Invalid number in {} line {}: {token} ({err})",
          path.display(),
          line_idx + 1
        )
      })?;
    }
    rows.push(values);
  }
  Ok(rows)
}

fn read_summary(path: &Path) -> Result<(f64, f64), String> {
  let contents =
    fs::read_to_string(path).map_err(|err| format!("Failed to read {}: {err}", path.display()))?;
  let mut iter = contents.split_whitespace();
  let x = iter
    .next()
    .ok_or_else(|| format!("Missing x_min in {}", path.display()))?
    .parse::<f64>()
    .map_err(|err| format!("Invalid x_min in {}: {err}", path.display()))?;
  let f = iter
    .next()
    .ok_or_else(|| format!("Missing f_min in {}", path.display()))?
    .parse::<f64>()
    .map_err(|err| format!("Invalid f_min in {}", path.display()))?;
  Ok((x, f))
}
