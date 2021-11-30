import style from "./Agent.module.css";

type Props = {
  name: string;
  onClick?: () => void;
};

export const Agent = ({ name, onClick }: Props) => {
  return (
    <a href="#" onClick={onClick} className={style.agent}>
      {name}
    </a>
  );
};

// TODO: tests
