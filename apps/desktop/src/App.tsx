import { useEffect, useMemo, useRef, useState } from "react";
import ReactECharts from "echarts-for-react";
import type { EChartsOption } from "echarts";
import { invoke } from "@tauri-apps/api/core";
import "./App.css";

type TaskKind = "minimize" | "differentiate";
type Task1Method = "dichotomy" | "golden";

type TaskParams = {
  task: TaskKind;
  func: string;
  a: string;
  b: string;
  task1Method: Task1Method;
  eps: string;
  h: string;
  rmseSweep: boolean;
};

type Task1Data = {
  func: [number, number][];
  points: [number, number, number][];
  intervals: [number, number, number, number][];
  xMin: number;
  fMin: number;
};

type Task2Data = {
  combined: [number, number, number, number, number, number][];
  rmse: [number, number, number, number][];
};

type TaskResponse = {
  task: TaskKind;
  task1?: Task1Data | null;
  task2?: Task2Data | null;
};

const DEFAULT_FORM: TaskParams = {
  task: "minimize",
  func: "sin(x) + x^3",
  a: "-2",
  b: "2",
  task1Method: "dichotomy",
  eps: "1e-4",
  h: "0.1",
  rmseSweep: true,
};

const SERIES_COLORS = ["#0f6d5d", "#d26a3a", "#2a4f87", "#b1496b"];

type Option = { label: string; value: string };

function DropdownField({
  label,
  value,
  options,
  onChange,
}: {
  label: string;
  value: string;
  options: Option[];
  onChange: (v: string) => void;
}) {
  const [open, setOpen] = useState(false);
  const ref = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    const handler = (e: MouseEvent) => {
      if (ref.current && !ref.current.contains(e.target as Node)) {
        setOpen(false);
      }
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, []);

  const current = options.find((o) => o.value === value);

  return (
    <label className="field">
      <span>{label}</span>
      <div className="dropdown" ref={ref}>
        <button
          type="button"
          className="dropdown-trigger"
          onClick={() => setOpen((v) => !v)}
          aria-haspopup="listbox"
          aria-expanded={open}
        >
          <span>{current ? current.label : "Select"}</span>
          <span className="dropdown-caret" />
        </button>
        {open ? (
          <div className="dropdown-menu" role="listbox">
            {options.map((opt) => (
              <div
                key={opt.value}
                className={`dropdown-option ${opt.value === value ? "selected" : ""}`}
                role="option"
                aria-selected={opt.value === value}
                onClick={() => {
                  onChange(opt.value);
                  setOpen(false);
                }}
              >
                {opt.label}
              </div>
            ))}
          </div>
        ) : null}
      </div>
    </label>
  );
}

function ChartCard({
  title,
  subtitle,
  option,
  height = 280,
}: {
  title: string;
  subtitle?: string;
  option: EChartsOption | null;
  height?: number;
}) {
  return (
    <div className="card">
      <div className="card-header">
        <div>
          <h3>{title}</h3>
          {subtitle ? <p>{subtitle}</p> : null}
        </div>
      </div>
      <div className="card-body">
        {option ? (
          <ReactECharts option={option} style={{ height }} />
        ) : (
          <div className="empty">Run a task to see data here.</div>
        )}
      </div>
    </div>
  );
}

function App() {
  const [form, setForm] = useState<TaskParams>(DEFAULT_FORM);
  const [result, setResult] = useState<TaskResponse | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const formatNumber = (value: number) => {
    if (!Number.isFinite(value)) return "—";
    const abs = Math.abs(value);
    if ((abs > 0 && abs < 1e-3) || abs >= 1e4) {
      return value.toExponential(3);
    }
    return value.toFixed(6);
  };

  const updateField = <K extends keyof TaskParams>(key: K, value: TaskParams[K]) => {
    setForm((prev) => ({ ...prev, [key]: value }));
  };

  const runTask = async () => {
    setLoading(true);
    setError(null);
    try {
      const response = await invoke<TaskResponse>("run_task", { params: form });
      setResult(response);
    } catch (err) {
      setError(String(err));
    } finally {
      setLoading(false);
    }
  };

  const task1Chart = useMemo<EChartsOption | null>(() => {
    const data = result?.task1;
    if (!data || data.func.length === 0) {
      return null;
    }
    const points = data.points.map((row) => [row[1], row[2]]);
    return {
      color: SERIES_COLORS,
      grid: { left: 40, right: 20, top: 40, bottom: 40 },
      tooltip: { trigger: "axis" },
      xAxis: { type: "value" },
      yAxis: { type: "value" },
      series: [
        {
          name: "f(x)",
          type: "line",
          showSymbol: false,
          data: data.func,
        },
        {
          name: "x*",
          type: "scatter",
          symbolSize: 8,
          data: points,
        },
      ],
    };
  }, [result]);

  const task1Summary = useMemo(() => {
    const data = result?.task1;
    if (!data || data.points.length === 0) {
      return null;
    }
    return {
      iter: data.points[data.points.length - 1][0],
      x: data.xMin,
      fx: data.fMin,
    };
  }, [result]);

  const task1IntervalChart = useMemo<EChartsOption | null>(() => {
    const data = result?.task1;
    if (!data || data.intervals.length === 0) {
      return null;
    }
    const seriesData = data.intervals.map((row) => [row[0], row[3]]);
    return {
      color: SERIES_COLORS,
      grid: { left: 40, right: 20, top: 40, bottom: 40 },
      tooltip: { trigger: "axis" },
      xAxis: { type: "value", name: "k" },
      yAxis: { type: "value", name: "length" },
      series: [
        {
          name: "interval",
          type: "line",
          showSymbol: false,
          data: seriesData,
        },
      ],
    };
  }, [result]);

  const task2FuncChart = useMemo<EChartsOption | null>(() => {
    const data = result?.task2;
    if (!data || data.combined.length === 0) {
      return null;
    }
    const func = data.combined.map((row) => [row[0], row[1]]);
    return {
      color: SERIES_COLORS,
      grid: { left: 40, right: 20, top: 40, bottom: 40 },
      tooltip: { trigger: "axis" },
      xAxis: { type: "value" },
      yAxis: { type: "value" },
      series: [
        {
          name: "f(x)",
          type: "line",
          showSymbol: false,
          data: func,
        },
      ],
    };
  }, [result]);

  const task2DerivativesChart = useMemo<EChartsOption | null>(() => {
    const data = result?.task2;
    if (!data || data.combined.length === 0) {
      return null;
    }
    const trueSeries = data.combined.map((row) => [row[0], row[2]]);
    const rightSeries = data.combined.map((row) => [row[0], row[3]]);
    const leftSeries = data.combined.map((row) => [row[0], row[4]]);
    const centralSeries = data.combined.map((row) => [row[0], row[5]]);
    return {
      color: SERIES_COLORS,
      grid: { left: 40, right: 20, top: 40, bottom: 40 },
      tooltip: { trigger: "axis" },
      xAxis: { type: "value" },
      yAxis: { type: "value" },
      legend: { top: 0 },
      series: [
        { name: "true", type: "line", showSymbol: false, data: trueSeries },
        { name: "right", type: "line", showSymbol: false, data: rightSeries },
        { name: "left", type: "line", showSymbol: false, data: leftSeries },
        { name: "central", type: "line", showSymbol: false, data: centralSeries },
      ],
    };
  }, [result]);

  const task2RmseChart = useMemo<EChartsOption | null>(() => {
    const data = result?.task2;
    if (!data || data.rmse.length === 0) {
      return null;
    }
    const rightSeries = data.rmse.map((row) => [row[0], row[1]]);
    const leftSeries = data.rmse.map((row) => [row[0], row[2]]);
    const centralSeries = data.rmse.map((row) => [row[0], row[3]]);
    return {
      color: SERIES_COLORS,
      grid: { left: 50, right: 20, top: 40, bottom: 40 },
      tooltip: { trigger: "axis" },
      xAxis: { type: "log", name: "h" },
      yAxis: { type: "log", name: "rmse" },
      legend: { top: 0 },
      series: [
        { name: "right", type: "line", showSymbol: true, data: rightSeries },
        { name: "left", type: "line", showSymbol: true, data: leftSeries },
        { name: "central", type: "line", showSymbol: true, data: centralSeries },
      ],
    };
  }, [result]);

  return (
    <div className="app">
      <header className="header">
        <div>
          <h1>Matan Lab</h1>
        </div>
        <button className="cta" onClick={runTask} disabled={loading}>
          {loading ? "Running..." : "Run"}
        </button>
      </header>

      <div className="layout">
        <aside className="panel">
          <div className="panel-section">
            <h2>Task</h2>
            <DropdownField
              label="Mode"
              value={form.task}
              options={[
                { label: "Minimize", value: "minimize" },
                { label: "Differentiate", value: "differentiate" },
              ]}
              onChange={(v) => updateField("task", v as TaskKind)}
            />
          </div>

          <div className="panel-section">
            <h2>Function</h2>
            <label className="field">
              <span>f(x)</span>
              <input
                value={form.func}
                onChange={(e) => updateField("func", e.target.value)}
              />
            </label>
            <label className="field">
              <span>a</span>
              <input value={form.a} onChange={(e) => updateField("a", e.target.value)} />
            </label>
            <label className="field">
              <span>b</span>
              <input value={form.b} onChange={(e) => updateField("b", e.target.value)} />
            </label>
          </div>

          {form.task === "minimize" ? (
            <div className="panel-section">
              <h2>Minimization</h2>
              <DropdownField
                label="Method"
                value={form.task1Method}
                options={[
                  { label: "Dichotomy", value: "dichotomy" },
                  { label: "Golden section", value: "golden" },
                ]}
                onChange={(v) => updateField("task1Method", v as Task1Method)}
              />
              <label className="field">
                <span>eps</span>
                <input
                  value={form.eps}
                  onChange={(e) => updateField("eps", e.target.value)}
                />
              </label>
            </div>
          ) : (
            <div className="panel-section">
              <h2>Differentiation</h2>
              <label className="field">
                <span>h</span>
                <input value={form.h} onChange={(e) => updateField("h", e.target.value)} />
              </label>
              <label className="field checkbox">
                <span>RMSE sweep</span>
                <input
                  type="checkbox"
                  checked={form.rmseSweep}
                  onChange={(e) => updateField("rmseSweep", e.target.checked)}
                />
              </label>
            </div>
          )}

          {error ? <div className="error">{error}</div> : null}

          <div className="panel-gif" aria-hidden />
        </aside>

        <section className="content">
          {form.task === "minimize" ? (
            <>
              <div className="card stats">
                <div className="stat">
                  <span>Iterations</span>
                  <strong>{task1Summary ? task1Summary.iter : "—"}</strong>
                </div>
                <div className="stat">
                  <span>x*</span>
                  <strong>{task1Summary ? formatNumber(task1Summary.x) : "—"}</strong>
                </div>
                <div className="stat">
                  <span>f(x*)</span>
                  <strong>{task1Summary ? formatNumber(task1Summary.fx) : "—"}</strong>
                </div>
              </div>
              <ChartCard
                title="Function and candidate minima"
                subtitle="f(x) with x* points"
                option={task1Chart}
                height={320}
              />
              <ChartCard
                title="Interval contraction"
                subtitle="Length of the uncertainty interval"
                option={task1IntervalChart}
              />
            </>
          ) : (
            <>
              <ChartCard
                title="Function"
                subtitle="Original f(x)"
                option={task2FuncChart}
                height={260}
              />
              <ChartCard
                title="Derivatives"
                subtitle="True vs numeric derivatives"
                option={task2DerivativesChart}
                height={320}
              />
              <ChartCard
                title="RMSE sweep"
                subtitle="Error vs step size"
                option={task2RmseChart}
                height={240}
              />
            </>
          )}
        </section>
      </div>
    </div>
  );
}

export default App;
