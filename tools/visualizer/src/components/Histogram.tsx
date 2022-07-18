import style from "./Histogram.module.css";
import * as types from "../types";
import lodash from "lodash";
import dayjs from "../dayjs";

type Props = {
  className?: string;
  histogram: types.Histogram;
  stats: types.Stats;
};

type HistogramParams = {
  max: number;
  total: number;
  bins: Map<number, number>;
};

function formatDuration(duration: number) {
  const d = dayjs.duration(duration, "seconds");
  if (d.months() > 0) return d.format("M[mo] D[d]");
  if (d.days() > 0) return d.format("D[d] H[h]");
  if (d.hours() > 0) return d.format("H[h] m[m]");
  return d.format("m [min]");
}

function inlineStyle(key: number, bins: Map<number, number>, max: number) {
  const height = (99 * (bins.get(key) || 0)) / max + 1;
  if (bins.has(key)) return { height: height + "%" };
  return {
    background: "transparent",
  };
}

function title(key: number, bins: Map<number, number>, total: number) {
  const binProb = ((100 * (bins.get(key) || 0)) / total).toFixed(2);
  return `${binProb}% (${formatDuration(key)})`;
}

function bin(
  key: number,
  { bins, max, total }: HistogramParams,
  hint?: number
) {
  let binStyle = style.bin;
  if (hint) binStyle += " " + style.binThreshold;

  return (
    <div
      key={key}
      className={binStyle}
      style={inlineStyle(key, bins, max)}
      title={title(key, bins, total)}
    >
      &nbsp;{hint ? <div className={style.hint}>{hint}%</div> : null}
    </div>
  );
}

function bins(histogram: types.Histogram, stats: types.Stats) {
  const start = histogram.bins[0][0];
  const end = stats.p95;
  const params: HistogramParams = {
    max: lodash.chain(histogram.bins).flatMap("[1]").max().value(),
    total: lodash.chain(histogram.bins).flatMap("[1]").sum().value(),
    bins: new Map(histogram.bins),
  };
  return lodash
    .range(start, end + histogram.bin_size, histogram.bin_size)
    .map((i) => {
      for (const threshold of [95, 75, 50, 25]) {
        const pVal = stats[("p" + threshold) as keyof types.Stats];
        if (i >= pVal && i < pVal + histogram.bin_size)
          return bin(i, params, threshold);
      }
      return bin(i, params);
    });
}

export const Histogram = ({ className, histogram, stats }: Props) => {
  const histogramStyle = [style.histogram, className].filter(Boolean).join(" ");
  return <div className={histogramStyle}>{bins(histogram, stats)}</div>;
};
