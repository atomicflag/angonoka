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

  expect(buckets[0].title).toBe("2.27% (0 min)");
  expect(buckets[0]).toHaveStyle({ height: "12%" });

  expect(buckets[3]).toHaveClass("bucketThreshold");
  expect(buckets[3].querySelector(".hint")).toHaveTextContent("25%");

  expect(buckets[4]).toHaveStyle({ background: "transparent" });

  expect(buckets[5]).toHaveStyle({ height: "100%" });
  expect(buckets[5]).toHaveClass("bucketThreshold");
  expect(buckets[5].querySelector(".hint")).toHaveTextContent("50%");
  expect(buckets[5].title).toBe("20.45% (5 min)");

  expect(buckets[7]).toHaveClass("bucketThreshold");
  expect(buckets[7].querySelector(".hint")).toHaveTextContent("75%");

  expect(buckets[8]).toHaveStyle({ height: "23%" });
  expect(buckets[8]).toHaveClass("bucketThreshold");
  expect(buckets[8].querySelector(".hint")).toHaveTextContent("95%");
  expect(buckets[8].title).toBe("4.55% (8 min)");
});