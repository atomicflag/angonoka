import { render, fireEvent } from "@testing-library/react";
import { AgentTimeline } from "./AgentTimeline";

const tasks = [
  {
    agent: "Agent",
    expected_duration: 50,
    expected_start: 0,
    priority: 0,
    task: "Task 1",
  },
  {
    agent: "Agent",
    expected_duration: 30,
    expected_start: 50,
    priority: 1,
    task: "Task 2",
  },
];

test("has tasks", () => {
  const { getByText } = render(<AgentTimeline tasks={tasks} makespan={70} />);

  expect(getByText("Task 1")).toBeInTheDocument();
  expect(getByText("Task 2")).toBeInTheDocument();
});

test("callback", () => {
  const callback = jest.fn();
  const { getByText } = render(
    <AgentTimeline tasks={tasks} makespan={70} onClick={callback} />
  );

  fireEvent.click(getByText("Task 2"));

  expect(callback).toHaveBeenCalledWith(tasks[1]);
});
