import { Task } from "../types";
import { TaskStrip } from "./TaskStrip";

type Props = {
  tasks: Task[];
  makespan: number;
};

export const AgentTimeline = ({ tasks, makespan }: Props) => {
  const taskStrips = tasks.map((v, i) => (
    <TaskStrip
      name={v.task}
      key={i}
      width={v.expected_duration / makespan}
      offset={v.expected_start / makespan}
    />
  ));
  return <div className="h-10 flex relative bg-white">{taskStrips}</div>;
};

// TODO: tests, css
