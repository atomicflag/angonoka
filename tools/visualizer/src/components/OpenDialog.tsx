import { Button } from "./Button";
import { Schedule } from "../types";
import { Dispatch, useState } from "react";
import { ScheduleUpload } from "./ScheduleUpload";
import { SchedulePaste } from "./SchedulePaste";
import style from "./OpenDialog.module.css";

type Props = {
  onOpen: (schedule: Schedule) => void;
};

function Opener(
  setIsVisible: Dispatch<boolean>,
  onOpen: (schedule: Schedule) => void
) {
  return (schedule: Schedule) => {
    setIsVisible(false);
    onOpen(schedule);
  };
}

// TODO: refactor
function dialog(
  isVisible: boolean,
  setIsVisible: Dispatch<boolean>,
  onOpen: (schedule: Schedule) => void
) {
  if (!isVisible) return;
  const opener = Opener(setIsVisible, onOpen);
  return (
    <div className={style.openDialog}>
      <div className={style.background} onClick={() => setIsVisible(false)}>
        &nbsp;
      </div>
      <div className={style.windowContainer}>
        <div className={style.window}>
          <a
            href="#"
            className={style.closeButton}
            onClick={() => setIsVisible(false)}
          >
            ×
          </a>
          <div className={style.layout}>
            <div className={style.upload}>
              <span className="mb-4">Upload an optimized schedule JSON</span>
              <ScheduleUpload onUpload={opener} />
            </div>
            <div className={style.separator}>─── or ───</div>
            <div className={style.clipboard}>
              <span className="mb-4 text-center">
                paste the schedule from clipboard
              </span>
              <SchedulePaste onPaste={opener} />
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export const OpenDialog = ({ onOpen }: Props) => {
  const [isVisible, setIsVisible] = useState<boolean>(false);
  return (
    <div>
      <Button text="Open" className="ml-2" onClick={() => setIsVisible(true)} />
      {dialog(isVisible, setIsVisible, onOpen)}
    </div>
  );
};

// TODO: test
