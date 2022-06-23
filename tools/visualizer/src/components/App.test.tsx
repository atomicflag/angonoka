import { within, render, fireEvent } from "@testing-library/react";
import { App } from "./App";
import lodash from "lodash";

const project = {
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
  const { getByText } = render(<App project={project} />);

  expect(getByText("Open")).toBeInTheDocument();

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
  const { queryByText, getByText } = render(<App project={project} />);

  expect(queryByText("Total tasks")).not.toBeInTheDocument();

  fireEvent.click(getByText("Agent 1"));

  expect(getByText("Total tasks")).toBeInTheDocument();

  const content = getByText("Total tasks").closest(".content").childNodes;
  const info = lodash.zip(
    Array.from(content[0].childNodes).map((v) => v.innerHTML),
    Array.from(content[1].childNodes).map((v) => v.innerHTML)
  );

  expect(info).toEqual([
    ["Total tasks", "1"],
    ["Total busy time", "3 minutes"],
    ["Total idle time", "None"],
    ["Utilization", "100%"],
  ]);

  fireEvent.click(getByText("×"));

  expect(queryByText("Total tasks")).not.toBeInTheDocument();
});

test("task info panel", () => {
  const { queryByText, getByText } = render(<App project={project} />);

  expect(queryByText("Priority")).not.toBeInTheDocument();

  fireEvent.click(getByText("Task 1"));

  expect(getByText("Priority")).toBeInTheDocument();

  const content = getByText("Priority").closest(".content").childNodes;
  const info = lodash.zip(
    Array.from(content[0].childNodes).map((v) => v.innerHTML),
    Array.from(content[1].childNodes).map((v) => v.innerHTML)
  );

  expect(info).toEqual([
    ["Duration", "3 minutes"],
    ["Priority", "1"],
  ]);

  fireEvent.click(getByText("×"));

  expect(queryByText("Priority")).not.toBeInTheDocument();
});
