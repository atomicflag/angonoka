import { Button } from "./Button";
import { RefObject, useRef } from "react";
import { Project } from "../types";

type Props = {
  onUpload: (project: Project) => void;
};

async function loadProject(
  fileUpload: RefObject<HTMLInputElement>,
  onUpload: (project: Project) => void
) {
  const fu = fileUpload.current;
  if (fu.files.length === 0) return;
  const text = await fu.files[0].text();
  fileUpload.current.value = "";
  onUpload(JSON.parse(text));
}

export const ProjectUpload = ({ onUpload }: Props) => {
  const fileUpload = useRef<HTMLInputElement>();
  return (
    <div>
      <Button text="Upload" onClick={() => fileUpload.current.click()} />
      <input
        type="file"
        ref={fileUpload}
        className="hidden"
        onInput={() => loadProject(fileUpload, onUpload)}
        accept=".json"
      />
    </div>
  );
};
