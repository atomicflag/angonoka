import { render } from "@testing-library/react";
import { MakespanBadge } from "./MakespanBadge";

test("renders duration", () => {
  const { getByText } = render(<MakespanBadge makespan={60} />);

  expect(getByText("a minute")).toBeInTheDocument();
});
