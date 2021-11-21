import React from "react";
import style from "./Agent.module.css";

type Props = {
  name: string;
};

export default class Agent extends React.Component<Props, {}> {
  render() {
    return <div className={style.agent}>{this.props.name}</div>;
  }
}

// TODO: tests
