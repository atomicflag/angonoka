import { Button } from "./Button";
import { RefObject, useRef } from "react";
import { Schedule } from "../types";

type Props = {
  onUpload: (schedule: Schedule) => void;
};

async function loadSchedule(
  fileUpload: RefObject<HTMLInputElement>,
  onUpload: (schedule: Schedule) => void
) {
  const fu = fileUpload.current;
  if (fu.files.length === 0) return;
  const text = await fu.files[0].text();
  fileUpload.current.value = "";
  onUpload(JSON.parse(text));
}

export const ScheduleUpload = ({ onUpload }: Props) => {
  const fileUpload = useRef<HTMLInputElement>();
  return (
    <div>
      <Button
        text="Load"
        className="ml-2"
        onClick={() => fileUpload.current.click()}
      />
      <input
        type="file"
        ref={fileUpload}
        className="hidden"
        onInput={() => loadSchedule(fileUpload, onUpload)}
        accept=".json"
      />
    </div>
  );
};
