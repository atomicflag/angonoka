import React from "react";
import style from "./Button.module.css";

type Props = {
  text: string;
  className?: string;
  onClick?: React.MouseEventHandler<HTMLButtonElement>;
};

export default class Button extends React.Component<Props, {}> {
  private get className() {
    return [style.button, this.props.className].filter(Boolean).join(" ");
  }

  render() {
    return (
      <button
        type="button"
        className={this.className}
        onClick={this.props.onClick}
      >
        {this.props.text}
      </button>
    );
  }
}
