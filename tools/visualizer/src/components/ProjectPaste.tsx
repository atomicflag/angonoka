import { Project } from "../types";
import { Button } from "./Button";
import { Dispatch, useState, RefObject, useRef } from "react";
import style from "./ProjectPaste.module.css";

type Props = {
  onPaste: (project: Project) => void;
};

function loadProject(
  text: RefObject<HTMLTextAreaElement>,
  onPaste: (project: Project) => void,
  setErrorText: Dispatch<string>
) {
  try {
    onPaste(JSON.parse(text.current.value));
  } catch (e) {
    if (e instanceof SyntaxError) {
      setErrorText("Invalid JSON");
    } else {
      throw e;
    }
  }
}

export const ProjectPaste = ({ onPaste }: Props) => {
  const text = useRef<HTMLTextAreaElement>();
  const [errorText, setErrorText] = useState<string>("");
  return (
    <div className={style.projectPaste}>
      <textarea className="text-black" ref={text} rows={4}></textarea>
      {errorText && <div className={style.errorText}>{errorText}</div>}
      <Button
        text="Load"
        className="mt-4"
        onClick={() => loadProject(text, onPaste, setErrorText)}
      />
    </div>
  );
};
