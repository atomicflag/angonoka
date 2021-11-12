import React from "react";
import { render } from "react-dom";

class App extends React.Component {
  render() {
    return (
      <div>
        <div className="bg-green-500 flex gap-2 p-4 text-white">
          <label className="self-center" htmlFor="scheduleJSON">
            Select schedule JSON:
          </label>
          <input
            className="flex-auto self-center"
            type="file"
            id="scheduleJSON"
            name="scheduleJSON"
          />
          <button
            type="button"
            className="bg-indigo-500 rounded py-2 px-4 self-center shadow font-semibold hover:bg-indigo-700"
          >
            Load
          </button>
        </div>
      </div>
    );
  }
}

export default function main() {
  const element = document.createElement("div");

  element.id = "root";

  document.body.appendChild(element);

  render(<App />, element);
}
