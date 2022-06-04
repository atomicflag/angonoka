import React from "react";
import style from "./Button.module.css";

type Props = {
  className?: string;
};

export const Histogram = ({ className}: Props) => {
  return (
    <div className={className}>
      Histogram
    </div>
  );
};
