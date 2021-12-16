import { render, fireEvent } from "@testing-library/react";
import { Agent } from "./Agent";

test("displays agent name", () => {
  const { getByText } = render(<Agent name="John Smith" />);

  expect(getByText("John Smith")).toBeInTheDocument();
});

test("callback", () => {
  const callback = jest.fn();
  const { getByText } = render(<Agent name="John Smith" onClick={callback} />);

  fireEvent.click(getByText("John Smith"));

  expect(callback).toHaveBeenCalled();
});
