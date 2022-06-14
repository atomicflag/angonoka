import "./react-refresh";
import "./global.css";
import "./styles.css";

import { App } from "./components/App";
import { createRoot } from "react-dom/client";

if (module.hot) {
  module.hot.accept();
}

function getRoot() {
  if (global.reactRoot) return global.reactRoot;
  const newRoot = document.createElement("div");
  document.body.appendChild(newRoot);
  global.reactRoot = createRoot(newRoot);
  return global.reactRoot;
}

getRoot().render(<App />);
