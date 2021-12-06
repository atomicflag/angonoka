import { render, fireEvent } from "@testing-library/react";
import { InfoPanel } from "./InfoPanel";

test("has title", () => {
  const { getByText } = render(
    <InfoPanel title="test" content={[["key", "value"]]} />
  );

  expect(getByText("test")).toBeInTheDocument();
  expect(getByText("key")).toBeInTheDocument();
  expect(getByText("value")).toBeInTheDocument();
});

test("callback", () => {
  const callback = jest.fn();
  const { getByText } = render(<InfoPanel title="test" onClose={callback} />);

  fireEvent.click(getByText("×"));

  expect(callback).toHaveBeenCalled();
});
