import React from "react";
import style from "./Button.module.css";

type Props = {
  text: string;
  className?: string;
  onClick?: React.MouseEventHandler<HTMLButtonElement>;
};

export const Button = ({ text, className, onClick }: Props) => {
  const buttonStyle = [style.button, className].filter(Boolean).join(" ");
  return (
    <button type="button" className={buttonStyle} onClick={onClick}>
      {text}
    </button>
  );
};
