import { render } from "react-dom";

export default function main() {
  const element = document.createElement("div");

  element.id = "root";

  document.body.appendChild(element);

  render(<h1 className="text-red-500">Hello, world!</h1>, element);
}
