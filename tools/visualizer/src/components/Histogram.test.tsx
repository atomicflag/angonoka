import { render, fireEvent } from "@testing-library/react";
import { Histogram } from "./Histogram";

const histogramData = {
  bucket_size: 60,
  buckets: [
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

test("has buckets", () => {
  const { container } = render(
    <Histogram histogram={histogramData} stats={histogramStats} />
  );

  const buckets = container.querySelectorAll(".bucket");

  expect(buckets).toHaveLength(9);
  expect(buckets[0]).toHaveStyle({
    height: "12%",
  });
  expect(buckets[4]).toHaveStyle({
    background: "transparent",
  });
  expect(buckets[5]).toHaveStyle({
    height: "100%",
  });
  expect(buckets[8]).toHaveStyle({
    height: "23%",
  });
});
