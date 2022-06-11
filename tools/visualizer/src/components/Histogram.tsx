import React from "react";
import style from "./Histogram.module.css";
import { Histogram, Stats } from "../types";
import lodash from "lodash";
import dayjs from "../dayjs";

type Props = {
  className?: string;
  histogram: Histogram;
  stats: Stats;
};

function formatDuration(duration: number) {
  const d = dayjs.duration(duration, "seconds");
  if (d.months() > 0) return d.format("M[mo] D[d]");
  if (d.days() > 0) return d.format("D[d] H[h]");
  if (d.hours() > 0) return d.format("H[h] m[m]");
  return d.format("m [min]");
}

type HistogramParams = {
  max: number;
  total: number;
  buckets: Map<number, number>;
};

function bucket(
  key: number,
  {buckets, max, total}: HistogramParams,
  hint: ?number
) {
  const height = (99 * (buckets.get(key) || 0)) / max + 1;
  let inlineStyle = {};
  if (buckets.has(key)) {
    inlineStyle["height"] = height + "%";
  } else {
    inlineStyle["background"] = "transparent";
  }
  let bucketStyle = style.bucket;
  if (hint)
    bucketStyle += " " + style.bucketThreshold;
  const likelihood = (100*(buckets.get(key) || 0)/total).toFixed(2);
  const title = `${likelihood}% (${formatDuration(key)})`;

  return (
    <div
      key={key}
      className={bucketStyle}
      style={inlineStyle}
      title={title}
    >
      &nbsp;{hint ? <div className={style.hint}>{hint}%</div> : null}
    </div>
  );
}


function buckets(histogram: Histogram, stats: Stats) {
  const start = histogram.buckets[0][0];
  const end = stats.p95;
  const params : HistogramParams = {
    max: lodash.chain(histogram.buckets).flatMap("[1]").max().value(),
    total: lodash.chain(histogram.buckets).flatMap("[1]").sum().value(),
    buckets: new Map(histogram.buckets)
  };
  return lodash
    .range(start, end + histogram.bucket_size, histogram.bucket_size)
    .map((i) => {
      for (let threshold of [95, 75, 50, 25]) {
        const pVal = stats["p" + threshold];
        if (i >= pVal && i < pVal + histogram.bucket_size)
          return bucket(i, params, threshold);
      }
      return bucket(i, params);
    });
}

export const Histogram = ({ className, histogram, stats }: Props) => {
  const histogramStyle = [style.histogram, className].filter(Boolean).join(" ");
  return <div className={histogramStyle}>{buckets(histogram, stats)}</div>;
};
