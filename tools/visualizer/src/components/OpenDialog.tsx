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

type ClosableProps = {
  setIsVisible: Dispatch<boolean>;
};

function Background({ setIsVisible }: ClosableProps) {
  return (
    <div className={style.background} onClick={() => setIsVisible(false)}>
      &nbsp;
    </div>
  );
}

function CloseButton({ setIsVisible }: ClosableProps) {
  return (
    <a
      href="#"
      className={style.closeButton}
      onClick={() => setIsVisible(false)}
    >
      ×
    </a>
  );
}

type OpenerProps = {
  opener: (schedule: Schedule) => void;
};

function Layout({ opener }: OpenerProps) {
  return (
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
  );
}

type DialogProps = {
  isVisible: boolean;
  setIsVisible: Dispatch<boolean>;
  onOpen: (schedule: Schedule) => void;
};

function Dialog({ isVisible, setIsVisible, onOpen }: DialogProps) {
  if (!isVisible) return null;
  const opener = Opener(setIsVisible, onOpen);
  return (
    <div className={style.openDialog}>
      <Background setIsVisible={setIsVisible} />
      <div className={style.windowContainer}>
        <div className={style.window}>
          <CloseButton setIsVisible={setIsVisible} />
          <Layout opener={opener} />
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
      <Dialog
        isVisible={isVisible}
        setIsVisible={setIsVisible}
        onOpen={onOpen}
      />
    </div>
  );
};

// TODO: test
