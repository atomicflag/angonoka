import React from "react";

type Props = {
  tasks: any; // TODO: type?
};

export default class AgentTimeline extends React.Component<Props, {}> {
  render() {
    return (
      <div className="h-10 flex items-center bg-white">TASKS TIMELINE</div>
    );
  }
}

// TODO: tests, css
