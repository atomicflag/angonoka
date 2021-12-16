import { render, fireEvent } from "@testing-library/react";
import { Button } from "./Button";

test("has text", () => {
  const { getByText } = render(<Button text="hello" />);

  const button = getByText("hello");
  expect(button.nodeName).toEqual("BUTTON");
  expect(button).toBeEnabled();
});

test("callback", () => {
  const callback = jest.fn();
  const { getByText } = render(<Button text="hello" onClick={callback} />);

  fireEvent.click(getByText("hello"));

  expect(callback).toHaveBeenCalled();
});
