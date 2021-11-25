import React from "react";
import style from "./App.module.css";
import { Button } from "./Button";
import Agent from "./Agent";
import AgentTimeline from "./AgentTimeline";
import { InfoPanel } from "./InfoPanel";
import lodash from "lodash";
import dayjs from "dayjs";
import duration from "dayjs/plugin/duration";
import relativeTime from "dayjs/plugin/relativeTime";
import { Schedule } from "../types";

dayjs.extend(duration);
dayjs.extend(relativeTime);

type State = {
  schedule: Schedule;
  isInfoPanelVisible: boolean;
};

const schedule = `
{
  "makespan": 720,
  "tasks": [
    {
      "agent": "Agent 3 Long Name",
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
    this.state = { schedule: JSON.parse(schedule), isInfoPanelVisible: false };
  }

  render() {
    const agentNames = this.agentNames();
    const agents = agentNames.map((v, i) => <Agent name={v} key={i} />);
    const agentTasks = this.agentTasks();
    const tasks = agentNames.map((v, i) => (
      <AgentTimeline
        tasks={agentTasks[v] || []}
        key={i}
        makespan={this.state.schedule.makespan}
      />
    ));
    return (
      <div className="flex flex-col">
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
          <div className="flex-grow"></div>
          <div>Makespan: {this.makespan()}</div>
        </div>
        <div className="flex p-4 gap-2">
          <div className="flex flex-col gap-2">{agents}</div>
          <div className="flex flex-col gap-2 flex-grow">{tasks}</div>
          {this.infoPanel()}
        </div>
      </div>
    );
  }

  private infoPanel() {
    if (this.state.isInfoPanelVisible)
      return (
        <InfoPanel
          onClose={() => this.setState({ isInfoPanelVisible: false })}
        />
      );
  }

  private makespan() {
    return dayjs.duration(this.state.schedule.makespan, "seconds").humanize();
  }

  private agentNames() {
    const tasks = this.state.schedule.tasks;
    return lodash.chain(tasks).map("agent").uniq().sort().value();
  }

  private agentTasks() {
    const tasks = this.state.schedule.tasks;
    return lodash
      .chain(tasks)
      .groupBy("agent")
      .mapValues((v) => lodash.sortBy(v, "priority"))
      .value();
  }

  private async loadSchedule() {
    const fu = this.fileUpload.current;
    if (fu.files.length === 0) return;
    const text = await fu.files[0].text();
    this.setState({ schedule: JSON.parse(text) });
  }
}

// TODO: tests
