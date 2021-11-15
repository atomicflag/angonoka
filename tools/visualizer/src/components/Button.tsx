import React from "react";
import style from "./Button.module.css";

interface IProps {
  text: string;
}

export default class Button extends React.Component<IProps, {}> {
  render() {
    return (
      <button type="button" className={style.button}>
        {this.props.text}
      </button>
    );
  }
}
