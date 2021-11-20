import React from "react";

type Props = {
  name: string;
  className?: string;
};

export default class Agent extends React.Component<Props, {}> {
  render() {
    return (
      <div className="bg-teal-500 text-white p-2 text-right">
        {this.props.name}
      </div>
    );
  }
}

// TODO: tests, css
