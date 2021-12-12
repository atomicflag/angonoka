import { within, render, fireEvent } from "@testing-library/react";
import { App } from "./App";

const schedule = {
  makespan: 180,
  tasks: [
    {
      agent: "Agent 1",
      expected_duration: 180,
      expected_start: 0,
      priority: 0,
      task: "Task 1",
    },
    {
      agent: "Agent 2",
      expected_duration: 180,
      expected_start: 0,
      priority: 0,
      task: "Task 2",
    },
  ],
};

test("App startup", () => {
  const { getByText } = render(<App schedule={schedule} />);

  expect(getByText("Load")).toBeInTheDocument();

  const makespan = getByText("Makespan");
  expect(makespan).toBeInTheDocument();
  expect(
    within(makespan.parentElement).getByText("3 minutes")
  ).toBeInTheDocument();

  expect(getByText("Agent 1")).toBeInTheDocument();
  expect(getByText("Agent 2")).toBeInTheDocument();
  expect(getByText("Task 1")).toBeInTheDocument();
  expect(getByText("Task 2")).toBeInTheDocument();
});

test("agent info panel", () => {
  const { queryByText, getByText } = render(<App schedule={schedule} />);

  expect(queryByText("Total tasks")).not.toBeInTheDocument();

  fireEvent.click(getByText("Agent 1"));

  expect(getByText("Total tasks")).toBeInTheDocument();
});
