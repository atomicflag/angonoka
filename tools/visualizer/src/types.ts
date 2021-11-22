export type Task = {
  agent: string;
  expected_duration: number;
  expected_start: number;
  priority: number;
  task: string;
};

export type Schedule = {
  makespan: number;
  tasks: Task[];
};
