import React from "react";
import style from "./Button.module.css";

interface IProps {
  text: string;
  className?: string;
}

export default class Button extends React.Component<IProps, {}> {
  private get className() {
    return [style.button, this.props.className].filter(Boolean).join(" ");
  }

  render() {
    return (
      <button type="button" className={this.className}>
        {this.props.text}
      </button>
    );
  }
}
