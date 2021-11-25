type Props = {
  onClose?: () => void;
  title?: string;
  content?: any;
};

export const InfoPanel = ({ onClose }: Props) => {
  return (
    <div className="bg-white max-w-sm rounded overflow-hidden shadow-md flex flex-col">
      <div className="px-4 py-1 bg-teal-900 text-white flex gap-2 items-center">
        <span className="flex-grow">header</span>
        <a href="#" className="text-3xl" onClick={onClose}>
          ×
        </a>
      </div>
      <div className="p-4">
        long long line that goes on forever and ever and ever and ever
        <br />
        content
        <br />
        content
        <br />
        content
        <br />
        content
        <br />
        content
        <br />
        content
        <br />
        content
        <br />
      </div>
    </div>
  );
};

// TODO: css, test
