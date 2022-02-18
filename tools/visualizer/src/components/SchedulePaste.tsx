import { Schedule } from "../types";
import { Button } from "./Button";
import { Dispatch, useState, RefObject, useRef } from "react";

type Props = {
  onPaste: (schedule: Schedule) => void;
};

function loadSchedule(
  text: RefObject<HTMLInputElement>,
  onPaste: (schedule: Schedule) => void,
  setErrorText: Dispatch<string>
) {
  try {
    onPaste(JSON.parse(text.current.value));
  } catch (e) {
    setErrorText("Invalid JSON");
  }
}

export const SchedulePaste = ({ onPaste }: Props) => {
  const text = useRef<HTMLInputElement>();
  const [errorText, setErrorText] = useState<string>("");
  return (
    <div className="flex flex-col items-stretch">
      <textarea className="text-black" ref={text} rows="4" />
      {errorText && (
        <div className="text-center text-red-500 mt-2">{errorText}</div>
      )}
      <Button
        text="Load"
        className="mt-4"
        onClick={() => loadSchedule(text, onPaste, setErrorText)}
      />
    </div>
  );
};

// TODO: test, css
