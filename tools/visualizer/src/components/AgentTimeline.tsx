import { Task } from "../types";
import { TaskStrip } from "./TaskStrip";
import style from "./AgentTimeline.module.css";

type Props = {
  tasks: Task[];
  makespan: number;
  onClick?: (v: Task) => void;
};

export const AgentTimeline = ({ tasks, makespan, onClick }: Props) => {
  const taskStrips = tasks.map((v, i) => (
    <TaskStrip
      name={v.task}
      key={i}
      onClick={() => onClick && onClick(v)}
      width={v.expected_duration / makespan}
      offset={v.expected_start / makespan}
    />
  ));
  return <div className={style.taskStrips}>{taskStrips}</div>;
};

// TODO: test
