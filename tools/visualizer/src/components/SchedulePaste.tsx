import { Schedule } from "../types";
import { Button } from "./Button";
import { Dispatch, useState, RefObject, useRef } from "react";
import style from "./SchedulePaste.module.css";

type Props = {
  onPaste: (schedule: Schedule) => void;
};

function loadSchedule(
  text: RefObject<HTMLTextAreaElement>,
  onPaste: (schedule: Schedule) => void,
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

export const SchedulePaste = ({ onPaste }: Props) => {
  const text = useRef<HTMLTextAreaElement>();
  const [errorText, setErrorText] = useState<string>("");
  return (
    <div className={style.schedulePaste}>
      <textarea className="text-black" ref={text} rows={4}></textarea>
      {errorText && <div className={style.errorText}>{errorText}</div>}
      <Button
        text="Load"
        className="mt-4"
        onClick={() => loadSchedule(text, onPaste, setErrorText)}
      />
    </div>
  );
};
