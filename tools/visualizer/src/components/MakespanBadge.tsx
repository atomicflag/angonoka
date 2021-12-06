import dayjs from "../dayjs";
import style from "./MakespanBadge.module.css";

type Props = {
  makespan: number;
};

export const MakespanBadge = ({ makespan }: Props) => {
  return (
    <div className={style.makespanBadge}>
      <div>Makespan</div>
      <div>{dayjs.duration(makespan, "seconds").humanize()}</div>
    </div>
  );
};
