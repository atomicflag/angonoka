import { render } from "@testing-library/react";
import { Button } from "./Button";

test("Button creation", () => {
  const { queryByText } = render(<Button text="hello" />);

  const button = queryByText("hello");
  expect(button.nodeName).toEqual("BUTTON");
  expect(button).toBeEnabled();
});
