import { Schedule } from "../types";
import { Button } from "./Button";

type Props = {
  onPaste: (schedule: Schedule) => void;
};

export const SchedulePaste = ({ onPaste }: Props) => {
  return (
    <div className="flex flex-col items-stretch">
      <textarea rows="4" />
      <Button text="Load" className="mt-4" onClick={() => {}} />
    </div>
  );
};
