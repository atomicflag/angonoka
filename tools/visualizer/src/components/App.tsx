import { Dispatch, RefObject, useRef, useState } from "react";
import style from "./App.module.css";
import { Button } from "./Button";
import { Agent } from "./Agent";
import { AgentTimeline } from "./AgentTimeline";
import { InfoPanel } from "./InfoPanel";
import lodash from "lodash";
import dayjs from "dayjs";
import duration from "dayjs/plugin/duration";
import relativeTime from "dayjs/plugin/relativeTime";
import { Schedule, Task } from "../types";

dayjs.extend(duration);
dayjs.extend(relativeTime);

const defaultSchedule = `
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

function makeInfoPanel(setInfoPanelVisible: Dispatch<boolean>) {
  return <InfoPanel onClose={() => setInfoPanelVisible(false)} />;
}

function makeMakespan(duration: number) {
  return <div>Makespan: {dayjs.duration(duration, "seconds").humanize()}</div>;
}

function agentNames(tasks: Task[]) {
  return lodash.chain(tasks).map("agent").uniq().sort().value();
}

function agentTasks(tasks: Task[]) {
  return lodash
    .chain(tasks)
    .groupBy("agent")
    .mapValues((v) => lodash.sortBy(v, "priority"))
    .value();
}

async function loadSchedule(
  fileUpload: RefObject<HTMLInputElement>,
  setSchedule: Dispatch<Schedule>
) {
  const fu = fileUpload.current;
  if (fu.files.length === 0) return;
  const text = await fu.files[0].text();
  setSchedule(JSON.parse(text));
  fileUpload.current.value = "";
}

export const App = () => {
  const fileUpload = useRef<HTMLInputElement>();
  const [schedule, setSchedule] = useState<Schedule>(
    JSON.parse(defaultSchedule)
  );
  const [isInfoPanelVisible, setInfoPanelVisible] = useState(true);

  // TODO: custom hooks?
  const names = agentNames(schedule?.tasks || []);
  const agents = names.map((v, i) => <Agent name={v} key={i} />);
  const tasks = agentTasks(schedule?.tasks || []);
  const timelines = names.map((v, i) => (
    <AgentTimeline
      tasks={tasks[v] || []}
      key={i}
      makespan={schedule.makespan}
    />
  ));

  return (
    <div className="flex flex-col">
      <div className={style.topBar}>
        <span className="text-lg font-medium">Schedule Visualizer v1</span>
        <Button
          text="Load"
          className="ml-2"
          onClick={() => fileUpload.current.click()}
        />
        <input
          type="file"
          ref={fileUpload}
          className="hidden"
          onInput={() => loadSchedule(fileUpload, setSchedule)}
          accept=".json"
        />
        <div className="flex-grow"></div>
        {schedule && makeMakespan(schedule.makespan)}
      </div>
      <div className="flex p-4 gap-2">
        <div className="flex flex-col gap-2">{agents}</div>
        <div className="flex flex-col gap-2 flex-grow">{timelines}</div>
        {isInfoPanelVisible && makeInfoPanel(setInfoPanelVisible)}
      </div>
    </div>
  );
};

// TODO: tests
