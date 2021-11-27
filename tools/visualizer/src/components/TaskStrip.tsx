import style from "./TaskStrip.module.css";

type Props = {
  name: string;
  width: number;
  offset: number;
};

export const TaskStrip = ({ name, width, offset }: Props) => {
  const inlineStyle = {
    width: width * 100 + "%",
    left: offset * 100 + "%",
  };
  return (
    <a href="#" className={style.taskStrip} style={inlineStyle}>
      <span className="mx-2">{name}</span>
    </a>
  );
};

// TODO: test, css, tooltip
