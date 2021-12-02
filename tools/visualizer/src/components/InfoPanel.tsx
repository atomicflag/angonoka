import style from "./InfoPanel.module.css";

type Props = {
  onClose?: () => void;
  title?: string;
  content?: React.ReactElement;
};

export const InfoPanel = ({ onClose, title, content }: Props) => {
  return (
    <div className={style.infoPanel}>
      <div className={style.header}>
        <span className="flex-grow">{title}</span>
        <a href="#" className="text-3xl" onClick={onClose}>
          ×
        </a>
      </div>
      <div className="p-4">{content}</div>
    </div>
  );
};

// TODO: css, test
