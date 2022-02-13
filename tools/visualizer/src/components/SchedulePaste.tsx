import { Schedule } from "../types";
import { Button } from "./Button";

type Props = {
  onPaste: (schedule: Schedule) => void;
};

export const SchedulePaste = ({ onPaste }: Props) => {
  return (
    <div>
      <Button
        text="Paste"
        className="ml-2"
        onClick={() => {}}
      />
      <div className=""></div>
    </div>
  );
};
