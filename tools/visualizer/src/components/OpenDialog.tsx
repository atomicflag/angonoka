import { Button } from "./Button";
import { Schedule } from "../types";
import { Dispatch, useState } from "react";

type Props = {
  onOpen: (schedule: Schedule) => void;
};

function dialog(isVisible: boolean, setIsVisible: Dispatch<boolean>) {
  if (!isVisible) return;
  return (
    <div className="fixed top-0 left-0 w-screen h-screen">
      <div
        className="fixed top-0 left-0 w-screen h-screen z-20 opacity-50 bg-black"
        onClick={() => setIsVisible(false)}
      >
        &nbsp;
      </div>
      <div className="fixed top-0 left-0 w-screen h-screen z-30 flex justify-center items-center pointer-events-none">
        <div className="bg-teal-900 lg:w-1/3 lg:h-2/3 h-full w-full rounded flex flex-col items-stretch justify-evenly divide-y divide-teal-500 p-4 pointer-events-auto">
          <div className="grow flex flex-col justify-center items-center">
            Upload
          </div>
          <div className="grow flex flex-col justify-center items-center">
            Paste text
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
      {dialog(isVisible, setIsVisible)}
    </div>
  );
};

// TODO: test, css
