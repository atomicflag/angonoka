import { render, fireEvent } from "@testing-library/react";
import { OpenDialog } from "./OpenDialog";

test("opens dialog window", () => {
  const { queryByText, getByText } = render(<OpenDialog onOpen={() => {}} />);

  expect(queryByText("Load")).not.toBeInTheDocument();
  expect(queryByText("Upload")).not.toBeInTheDocument();

  fireEvent.click(getByText("Open"));

  expect(queryByText("Load")).toBeInTheDocument();
  expect(queryByText("Upload")).toBeInTheDocument();
});

test("pressing X to close", () => {
  const { getByText, queryByText } = render(<OpenDialog onOpen={() => {}} />);

  fireEvent.click(getByText("Open"));

  fireEvent.click(getByText("×"));

  expect(queryByText("Load")).not.toBeInTheDocument();
  expect(queryByText("Upload")).not.toBeInTheDocument();
});

test("clicking outside to close", () => {
  const { getByText, queryByText, container } = render(
    <OpenDialog onOpen={() => {}} />
  );

  fireEvent.click(getByText("Open"));

  fireEvent.click(container.querySelector(".background"));

  expect(queryByText("Load")).not.toBeInTheDocument();
  expect(queryByText("Upload")).not.toBeInTheDocument();
});

test("pasting the project", (done) => {
  const callback = (data: any) => {
    expect(data).toEqual({ hello: "world" });
    done();
  };

  const { getByText, queryByText, container } = render(
    <OpenDialog onOpen={callback} />
  );

  fireEvent.click(getByText("Open"));

  const text = container.getElementsByTagName("TEXTAREA")[0];

  expect(text).toBeInTheDocument();

  fireEvent.change(text, {
    target: { value: '{"hello":"world"}' },
  });

  fireEvent.click(getByText("Load"));

  expect(queryByText("Load")).not.toBeInTheDocument();
  expect(queryByText("Upload")).not.toBeInTheDocument();
});
