import { Dispatch, useState } from "react";
import style from "./App.module.css";
import { Agent } from "./Agent";
import { AgentTimeline } from "./AgentTimeline";
import { InfoPanel } from "./InfoPanel";
import { ScheduleUpload } from "./ScheduleUpload";
import { MakespanBadge } from "./MakespanBadge";
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

type InfoPanelState = {
  isVisible: boolean;
  title?: string;
  content?: string[][];
};

function makeInfoPanel(
  state: InfoPanelState,
  setInfoPanelState: Dispatch<InfoPanelState>
) {
  return (
    <InfoPanel
      onClose={() => setInfoPanelState({ isVisible: false })}
      title={state.title}
      content={state.content}
      className="self-start w-80"
    />
  );
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

function formatDuration(duration: number) {
  if (duration < 1) return "None";
  return dayjs.duration(duration, "seconds").humanize();
}

function showAgentInfo(
  name: string,
  tasks: Task[],
  makespan: number,
  setInfoPanelState: Dispatch<InfoPanelState>
) {
  const durationBusy = tasks.reduce((a, v) => a + v.expected_duration, 0);
  const durationFree = makespan - durationBusy;
  setInfoPanelState({
    isVisible: true,
    title: name,
    content: [
      ["Total tasks", tasks.length.toString()],
      ["Total busy time", formatDuration(durationBusy)],
      ["Total idle time", formatDuration(durationFree)],
      ["Utilization", ((100 * durationBusy) / makespan).toFixed(0) + "%"],
    ],
  });
}

function showTaskInfo(task: Task, setInfoPanelState: Dispatch<InfoPanelState>) {
  setInfoPanelState({
    isVisible: true,
    title: task.task,
    content: [
      ["Duration", formatDuration(task.expected_duration)],
      ["Priority", (task.priority + 1).toString()],
    ],
  });
}

function agentsAndTimelines(
  setInfoPanelState: Dispatch<InfoPanelState>,
  schedule?: Schedule
) {
  if (!schedule) return [[], []];
  const names = agentNames(schedule.tasks);
  const tasks = agentTasks(schedule.tasks);
  const agents = names.map((v, i) => (
    <Agent
      name={v}
      key={i}
      onClick={() =>
        showAgentInfo(v, tasks[v] || [], schedule.makespan, setInfoPanelState)
      }
    />
  ));
  const timelines = names.map((v, i) => (
    <AgentTimeline
      tasks={tasks[v] || []}
      key={i}
      makespan={schedule.makespan}
      onClick={(v) => showTaskInfo(v, setInfoPanelState)}
    />
  ));
  return [agents, timelines];
}

export const App = () => {
  const [schedule, setSchedule] = useState<Schedule>(
    JSON.parse(defaultSchedule)
  );
  const [infoPanelState, setInfoPanelState] = useState<InfoPanelState>({
    isVisible: false,
  });

  const [agents, timelines] = agentsAndTimelines(setInfoPanelState, schedule);

  return (
    <div className="flex flex-col">
      <div className={style.topBar}>
        <span className="text-lg font-medium">Schedule Visualizer v1</span>
        <ScheduleUpload onUpload={setSchedule} />
        <div className="flex-grow"></div>
        {schedule && <MakespanBadge makespan={schedule.makespan} />}
      </div>
      <div className="flex p-4 gap-2">
        <div className="flex flex-col gap-2">{agents}</div>
        <div className="flex flex-col gap-2 flex-grow">{timelines}</div>
        {infoPanelState.isVisible &&
          makeInfoPanel(infoPanelState, setInfoPanelState)}
      </div>
    </div>
  );
};

// TODO: tests
