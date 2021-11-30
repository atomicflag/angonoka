import { render } from "@testing-library/react";
import { Agent } from "./Agent";

test("displays agent name", () => {
  const { getByText } = render(<Agent name="John Smith" />);

  expect(getByText("John Smith")).toBeInTheDocument();
});
