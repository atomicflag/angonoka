import React from "react";
import style from "./Histogram.module.css";
import { Histogram } from "../types";
import lodash from "lodash";

type Props = {
  className?: string;
  histogram: Histogram;
};

function bucket(height: number) {
  return <div className={style.bucket}>{height}</div>;
}

function buckets(histogram: Histogram) {
  const max = lodash.chain(histogram.buckets).flatMap("[1]").max().value();
  const buckets = new Map(histogram.buckets);
  const start = histogram.buckets[0][0];
  const end = lodash.last(histogram.buckets)[0];
  return lodash.range(start, end+histogram.bucket_size, histogram.bucket_size).map(i=>{
  const height = 100*(buckets.get(i) || 0)/max;
  const heightStyle = height == 0 ? {"background": "transparent"} : {"height": height+"%"};

  return <div key={i} className={style.bucket} style={heightStyle}>&nbsp;</div>;
  })
}

export const Histogram = ({ className, histogram }: Props) => {
  const histogramStyle = [style.histogram, className].filter(Boolean).join(" ");
  return (
    <div className={histogramStyle}>
      {buckets(histogram)}
    </div>
  );
};
