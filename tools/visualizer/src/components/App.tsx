import React from "react";
import style from "./App.module.css";

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
        <div className={style.cls + " flex gap-2 p-4 text-white"}>
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
          <button
            type="button"
            onClick={() => this.setState({ value: "asdf" })}
            className="bg-indigo-500 rounded py-2 px-4 self-center shadow font-semibold hover:bg-indigo-700"
          >
            Load
          </button>
        </div>
      </div>
    );
  }
}
