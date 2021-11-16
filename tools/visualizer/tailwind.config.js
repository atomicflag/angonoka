const colors = require("tailwindcss/colors");

module.exports = {
  mode: "jit",
  purge: ["src/**/*.tsx", "src/**/*.ts", "src/**/*.css"],
  darkMode: false, // or 'media' or 'class'
  theme: {
    extend: {
      colors: {
        teal: colors.teal,
        sky: colors.sky,
      },
    },
  },
  variants: {
    extend: {},
  },
  plugins: [],
};
