import { Button } from "./Button";
import { Project } from "../types";
import { Dispatch, useState } from "react";
import { ProjectUpload } from "./ProjectUpload";
import { ProjectPaste } from "./ProjectPaste";
import style from "./OpenDialog.module.css";

type Props = {
  onOpen: (project: Project) => void;
};

function Opener(
  setIsVisible: Dispatch<boolean>,
  onOpen: (project: Project) => void
) {
  return (project: Project) => {
    setIsVisible(false);
    onOpen(project);
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
  opener: (project: Project) => void;
};

function Layout({ opener }: OpenerProps) {
  return (
    <div className={style.layout}>
      <div className={style.upload}>
        <span className="mb-4">Upload time_estimation.json</span>
        <ProjectUpload onUpload={opener} />
      </div>
      <div className={style.separator}>─── or ───</div>
      <div className={style.clipboard}>
        <span className="mb-4 text-center">
          paste JSON from clipboard
        </span>
        <ProjectPaste onPaste={opener} />
      </div>
    </div>
  );
}

type DialogProps = {
  isVisible: boolean;
  setIsVisible: Dispatch<boolean>;
  onOpen: (project: Project) => void;
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
