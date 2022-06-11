import "./react-refresh";
import "./global.css";
import "./styles.css";

import { App } from "./components/App";
import { createRoot } from "react-dom/client";

if (module.hot) {
  module.hot.accept();
}

function getRoot() {
  const root = document.getElementById("root");
  if (root) return root._reactRoot;

  const newRoot = document.createElement("div");
  newRoot.id = "root";

  document.body.appendChild(newRoot);
  // Not sure how to do this properly
  newRoot._reactRoot = createRoot(newRoot);
  return newRoot._reactRoot;
}

getRoot().render(<App />);
