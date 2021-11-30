import { render } from "@testing-library/react";
import { App } from "./App";

test("App startup", () => {
  const { getByText } = render(<App />);

  expect(getByText("Load")).toBeInTheDocument();
});
