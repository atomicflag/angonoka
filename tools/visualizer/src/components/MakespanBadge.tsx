import dayjs from "dayjs";

type Props = {
  makespan: number;
};

export const MakespanBadge = ({ makespan }: Props) => {
  return (
    <div className="border border-green-200 bg-green-200 flex">
      <div className="text-teal-900 px-2">Makespan</div>
      <div className="px-2 bg-teal-900">
        {dayjs.duration(makespan, "seconds").humanize()}
      </div>
    </div>
  );
};

// TODO: test
