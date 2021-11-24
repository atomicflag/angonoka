import React from "react";
import style from "./TaskStrip.module.css";

type Props = {
  name: string;
  width: number;
  offset: number;
};

export default class TaskStrip extends React.Component<Props, {}> {
  render() {
    const inlineStyle = {
      width: this.props.width * 100 + "%",
      left: this.props.offset * 100 + "%",
    };
    return (
      <div className={style.taskStrip} style={inlineStyle}>
        <span className="mx-2">{this.props.name}</span>
      </div>
    );
  }
}

// TODO: test, css, tooltip
