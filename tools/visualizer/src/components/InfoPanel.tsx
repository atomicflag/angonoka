import style from "./InfoPanel.module.css";

type Props = {
  onClose?: () => void;
  title?: string;
  content?: string[][];
  className?: string;
};

function renderInfo(data: string[][]) {
  const titles = data.map(([v], i) => (
    <div className="text-right text-gray-500" key={i}>
      {v}
    </div>
  ));
  const descriptions = data.map(([, v], i) => (
    <div className="px-2" key={i}>
      {v}
    </div>
  ));
  return (
    <div className="p-6 flex justify-center">
      <div className="flex flex-col">{titles}</div>
      <div className="flex flex-col">{descriptions}</div>
    </div>
  );
}

export const InfoPanel = ({ onClose, title, content, className }: Props) => {
  const panelStyle = [style.infoPanel, className].filter(Boolean).join(" ");
  return (
    <div className={panelStyle}>
      <div className={style.header}>
        <span className="flex-grow truncate">{title}</span>
        <a href="#" className="text-3xl" onClick={onClose}>
          ×
        </a>
      </div>
      {renderInfo(content)}
    </div>
  );
};

// TODO: css, test
