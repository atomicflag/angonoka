import { render } from "@testing-library/react";
import App from "./App";

test("App startup", () => {
  const { queryByText } = render(<App />);

  expect(queryByText("Load")).toBeTruthy();
});
