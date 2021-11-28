import { render } from "@testing-library/react";
import { Agent } from "./Agent";

test("displays agent name", () => {
  const { queryByText } = render(<Agent name="John Smith" />);

  expect(queryByText("John Smith")).toBeTruthy();
});
