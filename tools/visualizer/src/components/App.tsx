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
        <div className={style.cls + " flex gap-2 p-2 text-white"}>
          <label className="self-center" htmlFor="scheduleJSON">
            Select schedule JSON:
          </label>
          <span>{this.state.value}</span>
          <input
            className="flex-auto self-center"
            type="file"
            id="scheduleJSON"
            name="scheduleJSON"
          />
          <Button text="Load" />
        </div>
      </div>
    );
  }
}
