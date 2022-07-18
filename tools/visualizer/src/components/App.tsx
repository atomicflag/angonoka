import { Dispatch, useState } from "react";
import style from "./App.module.css";
import { Agent } from "./Agent";
import { AgentTimeline } from "./AgentTimeline";
import { Histogram } from "./Histogram";
import { InfoPanel } from "./InfoPanel";
import { OpenDialog } from "./OpenDialog";
import { MakespanBadge } from "./MakespanBadge";
import dayjs from "../dayjs";
import lodash from "lodash";
import { Project, Task } from "../types";

type Props = {
  project?: Project;
};

const defaultProject = `
{
  "makespan": 720,
  "tasks": [
    {
      "agent": "Agent 3 Long Name",
      "expected_duration": 180,
      "expected_start": 0,
      "priority": 0,
      "task": "Task 1"
    },
    {
      "agent": "Agent 2",
      "expected_duration": 600,
      "expected_start": 0,
      "priority": 0,
      "task": "Task 2"
    },
    {
      "agent": "Agent 1",
      "expected_duration": 330,
      "expected_start": 0,
      "priority": 0,
      "task": "Task 3"
    },
    {
      "agent": "Agent 1",
      "expected_duration": 390,
      "expected_start": 330,
      "priority": 1,
      "task": "Task 4"
    }
  ],
  "histogram": {
    "bin_size": 60,
    "bins": [
      [0, 1],
      [60, 3],
      [120, 7],
      [180, 8],
      [300, 9],
      [360, 8],
      [420, 5],
      [480, 2],
      [540, 1]
    ]
  },
  "stats": {
    "p25": 180,
    "p50": 300,
    "p75": 420,
    "p95": 480
  }
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
  project?: Project
) {
  if (!project) return [[], []];
  const names = agentNames(project.tasks);
  const tasks = agentTasks(project.tasks);
  const agents = names.map((v, i) => (
    <Agent
      name={v}
      key={i}
      onClick={() =>
        showAgentInfo(v, tasks[v] || [], project.makespan, setInfoPanelState)
      }
    />
  ));
  const timelines = names.map((v, i) => (
    <AgentTimeline
      tasks={tasks[v] || []}
      key={i}
      makespan={project.makespan}
      onClick={(v) => showTaskInfo(v, setInfoPanelState)}
    />
  ));
  return [agents, timelines];
}

export const App = (props: Props) => {
  const [project, setProject] = useState<Project>(
    props.project || JSON.parse(defaultProject)
  );
  const [infoPanelState, setInfoPanelState] = useState<InfoPanelState>({
    isVisible: false,
  });

  const [agents, timelines] = agentsAndTimelines(setInfoPanelState, project);

  return (
    <div className="flex flex-col">
      <div className={style.topBar}>
        <span className="text-lg font-medium pr-2">Angonoka Visualizer v3</span>
        <OpenDialog onOpen={setProject} />
        <div className="flex-grow"></div>
        {project && <MakespanBadge makespan={project.makespan} />}
      </div>
      {project.histogram && (
        <Histogram histogram={project.histogram} stats={project.stats} />
      )}
      <div className="flex p-4 gap-2">
        <div className="flex flex-col gap-2">{agents}</div>
        <div className="flex flex-col gap-2 flex-grow">{timelines}</div>
        {infoPanelState.isVisible &&
          makeInfoPanel(infoPanelState, setInfoPanelState)}
      </div>
    </div>
  );
};
