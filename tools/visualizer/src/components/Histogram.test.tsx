import { render, fireEvent } from "@testing-library/react";
import { Histogram } from "./Histogram";

const histogramData = {
  bin_size: 60,
  bins: [
    [0, 1],
    [60, 3],
    [120, 7],
    [180, 8],
    [300, 9],
    [360, 8],
    [420, 5],
    [480, 2],
    [540, 1],
  ],
};

const histogramStats = {
  p25: 180,
  p50: 300,
  p75: 420,
  p95: 480,
};

test("has bins", () => {
  const { container } = render(
    <Histogram histogram={histogramData} stats={histogramStats} />
  );

  const bins = container.querySelectorAll(".bin");

  expect(bins).toHaveLength(9);

  expect(bins[0].title).toBe("2.27% (0 min)");
  expect(bins[0]).toHaveStyle({ height: "12%" });

  expect(bins[3]).toHaveClass("binThreshold");
  expect(bins[3].querySelector(".hint")).toHaveTextContent("25%");

  expect(bins[4]).toHaveStyle({ background: "transparent" });

  expect(bins[5]).toHaveStyle({ height: "100%" });
  expect(bins[5]).toHaveClass("binThreshold");
  expect(bins[5].querySelector(".hint")).toHaveTextContent("50%");
  expect(bins[5].title).toBe("20.45% (5 min)");

  expect(bins[7]).toHaveClass("binThreshold");
  expect(bins[7].querySelector(".hint")).toHaveTextContent("75%");

  expect(bins[8]).toHaveStyle({ height: "23%" });
  expect(bins[8]).toHaveClass("binThreshold");
  expect(bins[8].querySelector(".hint")).toHaveTextContent("95%");
  expect(bins[8].title).toBe("4.55% (8 min)");
});
