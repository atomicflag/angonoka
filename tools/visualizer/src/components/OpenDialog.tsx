import { Button } from "./Button";
import { Schedule } from "../types";
import { Dispatch, useState } from "react";
import { ScheduleUpload } from "./ScheduleUpload";
import { SchedulePaste } from "./SchedulePaste";

type Props = {
  onOpen: (schedule: Schedule) => void;
};

function dialog(
  isVisible: boolean,
  setIsVisible: Dispatch<boolean>,
  onOpen: (schedule: Schedule) => void
) {
  if (!isVisible) return;
  return (
    <div className="fixed top-0 left-0 w-screen h-screen z-20">
      <div
        className="fixed top-0 left-0 w-screen h-screen z-20 opacity-50 bg-black"
        onClick={() => setIsVisible(false)}
      >
        &nbsp;
      </div>
      <div className="fixed top-0 left-0 w-screen h-screen z-30 flex flex-col justify-center items-center pointer-events-none">
        <div className="bg-teal-900 lg:w-1/3 lg:h-2/3 h-full w-full rounded p-8 pointer-events-auto shadow-md flex flex-col items-stretch justify-stretch absolute">
          <a
            href="#"
            className="absolute top-0 right-0 pr-4 pt-2 text-2xl"
            onClick={() => setIsVisible(false)}
          >
            ×
          </a>
          <div className="flex flex-col items-stretch justify-evenly h-full">
            <div className="flex flex-col justify-center items-center">
              <span className="mb-4">Upload an optimized schedule JSON</span>
              <ScheduleUpload onUpload={onOpen} />
            </div>
            <div className="flex justify-center items-center">─── or ───</div>
            <div className="flex flex-col justify-center items-stretch">
              <span className="mb-4 text-center">
                paste the schedule from clipboard
              </span>
              <SchedulePaste onPaste={onOpen} />
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export const OpenDialog = ({ onOpen }: Props) => {
  const [isVisible, setIsVisible] = useState<boolean>(true);
  return (
    <div>
      <Button text="Open" className="ml-2" onClick={() => setIsVisible(true)} />
      {dialog(isVisible, setIsVisible, onOpen)}
    </div>
  );
};

// TODO: test, css
