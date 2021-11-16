import React from "react";
import style from "./App.module.css";
import Button from "./Button";

interface IState {
  value: string;
}

export class App extends React.Component<{}, IState> {
  constructor(props: {}) {
    super(props);
    this.state = {
      value: "",
    };
  }

  render() {
    return (
      <div>
        <div className={style.topBar}>
          <span className="text-lg font-medium">Schedule Visualizer v1</span>
          <Button text="Load" className="ml-2" />
        </div>
      </div>
    );
  }
}
