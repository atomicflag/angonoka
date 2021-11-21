import React from "react";

// TODO: move to a dedicated module
type Task = {
  agent: string;
  expected_duration: number;
  expected_start: number;
  priority: number;
  task: string;
};

type Props = {
  tasks: Task[];
};

export default class AgentTimeline extends React.Component<Props, {}> {
  render() {
    // TODO: handle empty tasks array
    return (
      <div className="h-10 flex items-center bg-white">
        Tasks for {this.props.tasks[0].agent}, {this.props.tasks.length} total
      </div>
    );
  }
}

// TODO: tests, css
