import { render, fireEvent } from "@testing-library/react";
import { ScheduleUpload } from "./ScheduleUpload";

test("has text", () => {
  const { getByText } = render(<ScheduleUpload onUpload={() => {}} />);

  const button = getByText("Load");
  expect(button.nodeName).toEqual("BUTTON");
  expect(button).toBeEnabled();
});

test("callback", (done) => {
  const callback = (data: any) => {
    expect(data).toEqual({ hello: "world" });
    done();
  };
  const { container, getByText } = render(
    <ScheduleUpload onUpload={callback} />
  );

  const input = container.getElementsByTagName("INPUT")[0];
  expect(input).toBeInTheDocument();

  fireEvent.click(getByText("Load"));

  const file = {
    text: async () => '{"hello":"world"}',
  };
  fireEvent.change(input, {
    target: { files: [file] },
  });
  fireEvent.input(input);
});
