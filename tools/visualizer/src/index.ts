import "./global.css";
import "./styles.css";

import App from "./components/App";
import { render } from "react-dom";
import React from "react";

if (module.hot) {
  module.hot.accept();
}

function getRoot() {
  const root = document.getElementById("root");
  if (root) return root;

  const newRoot = document.createElement("div");
  newRoot.id = "root";

  document.body.appendChild(newRoot);
  return newRoot;
}

render(React.createElement(App), getRoot());
