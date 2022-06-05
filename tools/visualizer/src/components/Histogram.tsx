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
  let items = [];
  // TODO: use lodash.range instead
  // TODO: use i as react's key for items
  for(let i = start; i <= end; i += histogram.bucket_size) {
  console.info(i);
  }
  return histogram.buckets.map(b=>bucket(b[1]));
}

export const Histogram = ({ className, histogram }: Props) => {
  const histogramStyle = [style.histogram, className].filter(Boolean).join(" ");
  return (
    <div className={histogramStyle}>
      {buckets(histogram)}
    </div>
  );
};
