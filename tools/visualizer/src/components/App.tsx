import React from "react";
import style from "./App.module.css";
import Button from "./Button";

type State = {
  schedule: any;
};

export default class App extends React.Component<{}, State> {
  fileUpload: React.RefObject<HTMLInputElement>;

  constructor(props: {}) {
    super(props);
    this.fileUpload = React.createRef();
  }

  render() {
    return (
      <div>
        <div className={style.topBar}>
          <span className="text-lg font-medium">Schedule Visualizer v1</span>
          <Button
            text="Load"
            className="ml-2"
            onClick={this.loadSchedule.bind(this)}
          />
          <input type="file" ref={this.fileUpload} className="hidden" />
        </div>
      </div>
    );
  }

  private loadSchedule() {
    this.fileUpload.current.click();
  }
}
