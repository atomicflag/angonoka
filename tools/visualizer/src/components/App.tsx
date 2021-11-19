import React from "react";
import style from "./App.module.css";
import Button from "./Button";

type State = {
  schedule: any;
};

const schedule = `
{
  "makespan": 720,
  "tasks": [
    {
      "agent": "Agent 3",
      "expected_duration": 180,
      "expected_start": 0.0,
      "priority": 0,
      "task": "Task 1"
    },
    {
      "agent": "Agent 2",
      "expected_duration": 600,
      "expected_start": 0.0,
      "priority": 0,
      "task": "Task 2"
    },
    {
      "agent": "Agent 1",
      "expected_duration": 330,
      "expected_start": 0.0,
      "priority": 0,
      "task": "Task 3"
    },
    {
      "agent": "Agent 1",
      "expected_duration": 390,
      "expected_start": 330.0,
      "priority": 1,
      "task": "Task 4"
    }
  ]
}
`;

export default class App extends React.Component<{}, State> {
  fileUpload: React.RefObject<HTMLInputElement>;

  constructor(props: {}) {
    super(props);
    this.fileUpload = React.createRef();
    // Temporary hardcode the schedule so that
    // we don't have to load it each time.
    // TODO: Remove this
    this.state = { schedule: JSON.parse(schedule) };
  }

  render() {
    return (
      <div>
        <div className={style.topBar}>
          <span className="text-lg font-medium">Schedule Visualizer v1</span>
          <Button
            text="Load"
            className="ml-2"
            onClick={() => this.fileUpload.current.click()}
          />
          <input
            type="file"
            ref={this.fileUpload}
            className="hidden"
            onChange={this.loadSchedule.bind(this)}
            accept=".json"
          />
        </div>
      </div>
    );
  }

  private async loadSchedule() {
    const fu = this.fileUpload.current;
    if (fu.files.length === 0) return;
    const text = await fu.files[0].text();
    this.setState({ schedule: JSON.parse(text) });
  }
}
