import React from "react";

type Props = {
  name: string;
};

export default class Agent extends React.Component<Props, {}> {
  render() {
    return (
      <div className="bg-teal-500 h-10 flex items-center justify-end text-white p-2 text-right">
        {this.props.name}
      </div>
    );
  }
}

// TODO: tests, css
