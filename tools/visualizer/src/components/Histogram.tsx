import React from "react";
import style from "./Histogram.module.css";
import { Histogram, Stats } from "../types";
import lodash from "lodash";

type Props = {
  className?: string;
  histogram: Histogram;
  stats: Stats;
};

function bucket(key: number, buckets: Map<number, number>, max: number, hint: number?) {
  const height = 99*(buckets.get(key) || 0)/max + 1;
  const heightStyle = !buckets.has(key) ? {"background": "transparent"} : {"height": height+"%"};

  return <div key={key} className={style.bucket} style={heightStyle}>&nbsp;{hint ? <div className={style.hint}>{hint}%</div> : null}</div>;
}

function buckets(histogram: Histogram, stats: Stats) {
  const max = lodash.chain(histogram.buckets).flatMap("[1]").max().value();
  const buckets = new Map(histogram.buckets);
  const start = histogram.buckets[0][0];
  const end = stats.p95;
  // TODO: add p25, p50, p75, p95 hints
  return lodash.range(start, end+histogram.bucket_size, histogram.bucket_size).map(i=> bucket(i, buckets, max));
}

export const Histogram = ({ className, histogram, stats }: Props) => {
  const histogramStyle = [style.histogram, className].filter(Boolean).join(" ");
  return (
    <div className={histogramStyle}>
      {buckets(histogram, stats)}
    </div>
  );
};
