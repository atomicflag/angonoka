import { Button } from "./Button";
import { Schedule } from "../types";

type Props = {
  onOpen: (schedule: Schedule) => void;
};

export const OpenDialog = ({ onOpen }: Props) => {
  return (
    <div>
      <Button
        text="Open"
        className="ml-2"
        onClick={() => {}}
      />
      <div className="fixed top-0 left-0 w-screen h-screen z-20 opacity-50 bg-black">&nbsp;</div>
      <div className="fixed top-0 left-0 w-screen h-screen z-30 flex justify-center items-center">
        <div className="bg-teal-900 lg:w-1/3 lg:h-2/3 h-full w-full rounded flex flex-col items-stretch justify-evenly divide-y divide-teal-500 p-4">
          <div className="grow flex flex-col justify-center items-center">Upload</div>
          <div className="grow flex flex-col justify-center items-center">Paste text</div>
        </div>
      </div>
    </div>
  );
};

// TODO: test, css
