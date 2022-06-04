import React from "react";
import style from "./Histogram.module.css";
import { Histogram } from "../types";

type Props = {
  className?: string;
  histogram: Histogram;
};

function bucket(height: number) {
  return <div>{height}</div>;
}

function buckets(histogram: Histogram) {
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
