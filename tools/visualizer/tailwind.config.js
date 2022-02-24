const colors = require("tailwindcss/colors");

module.exports = {
  content: ["src/**/*.{ts,tsx,css}"],
  theme: {
    extend: {
      colors: {
        teal: colors.teal,
        sky: colors.sky,
      },
    },
  },
  plugins: [],
};
