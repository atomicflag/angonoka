import style from "./Agent.module.css";

type Props = {
  name: string;
};

export const Agent = ({ name }: Props) => {
  return (
    <a href="#" className={style.agent}>
      {name}
    </a>
  );
};

// TODO: tests
