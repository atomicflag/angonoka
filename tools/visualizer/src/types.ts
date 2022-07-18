export type Task = {
  agent: string;
  expected_duration: number;
  expected_start: number;
  priority: number;
  task: string;
};

export type Histogram = {
  bin_size: number;
  bins: [number, number][];
};

export type Stats = {
  p25: number;
  p50: number;
  p75: number;
  p95: number;
  p99: number;
};

export type Project = {
  makespan: number;
  tasks: Task[];
  histogram?: Histogram;
  stats?: Stats;
};
