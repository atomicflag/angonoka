import { render, fireEvent } from "@testing-library/react";
import { OpenDialog } from "./OpenDialog";

test("opens dialog window", () => {
  const { queryByText, getByText } = render(<OpenDialog onOpen={() => {}} />);

  expect(queryByText("Load")).not.toBeInTheDocument();
  expect(queryByText("Upload")).not.toBeInTheDocument();

  fireEvent.click(getByText("Open"));

  expect(queryByText("Load")).toBeInTheDocument();
  expect(queryByText("Upload")).toBeInTheDocument();
});

// TODO: add more tests
