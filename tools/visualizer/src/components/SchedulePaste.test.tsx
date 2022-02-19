import { render, fireEvent } from "@testing-library/react";
import { SchedulePaste } from "./SchedulePaste";

test("has text", () => {
  const { getByText } = render(<SchedulePaste onPaste={() => {}} />);

  const button = getByText("Load");
  expect(button.nodeName).toEqual("BUTTON");
  expect(button).toBeEnabled();
});

test("callback", (done) => {
  const callback = (data: any) => {
    expect(data).toEqual({ hello: "world" });
    done();
  };
  const { container, getByText } = render(<SchedulePaste onPaste={callback} />);

  const text = container.getElementsByTagName("TEXTAREA")[0];
  expect(text).toBeInTheDocument();

  fireEvent.change(text, {
    target: { value: '{"hello":"world"}' },
  });

  fireEvent.click(getByText("Load"));
});

test("has an error message", () => {
  const { container, getByText } = render(<SchedulePaste onPaste={() => {}} />);

  const text = container.getElementsByTagName("TEXTAREA")[0];

  fireEvent.change(text, {
    target: { value: "asdf" },
  });
  fireEvent.click(getByText("Load"));

  expect(getByText("Invalid JSON")).toBeInTheDocument();
});
