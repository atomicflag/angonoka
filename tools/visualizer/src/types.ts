export type Task = {
  agent: string;
  expected_duration: number;
  expected_start: number;
  priority: number;
  task: string;
};

export type Histogram = {
  bucket_size: number;
  buckets: [number, number][];
};

export type Project = {
  makespan: number;
  tasks: Task[];
  histogram: Histogram?;
};
