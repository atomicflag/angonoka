import React from "react";
import { Task } from "../types";
import TaskStrip from "./TaskStrip";

type Props = {
  tasks: Task[];
  makespan: number;
};

export default class AgentTimeline extends React.Component<Props, {}> {
  render() {
    return (
      <div className="h-10 flex relative bg-white">{this.taskStrips()}</div>
    );
  }

  private taskStrips() {
    const total = this.props.makespan;
    return this.props.tasks.map((v, i) => (
      <TaskStrip
        name={v.task}
        key={i}
        width={v.expected_duration / total}
        offset={v.expected_start / total}
      />
    ));
  }
}

// TODO: tests, css
