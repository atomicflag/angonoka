const isProductionMode = process.env.NODE_ENV === "production";

const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const ReactRefreshTypeScript = require("react-refresh-typescript");

module.exports = {
  devtool: isProductionMode ? false : "eval-source-map",
  name: "main",
  mode: isProductionMode ? "production" : "development",
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: [
          {
            loader: "ts-loader",
            options: {
              getCustomTransformers: () => ({
                before: !isProductionMode ? [ReactRefreshTypeScript()] : [],
              }),
              transpileOnly: !isProductionMode,
            },
          },
        ],
        exclude: /node_modules/,
      },
      {
        test: /\.css$/i,
        use: [
          isProductionMode ? MiniCssExtractPlugin.loader : "style-loader",
          {
            loader: "css-loader",
            options: {
              modules: {
                auto: true,
                localIdentName: "[local]--[hash:base64:5]",
              },
            },
          },
          "postcss-loader",
        ],
      },
    ],
  },
  resolve: {
    extensions: [".tsx", ".ts", ".js"],
  },
  plugins: [
    new MiniCssExtractPlugin({
      filename: isProductionMode ? "[name].[contenthash].css" : "[name].css",
    }),
    new HtmlWebpackPlugin({
      title: "Schedule Visualizer",
    }),
  ],
  optimization: isProductionMode
    ? {
        runtimeChunk: "single",
        moduleIds: "deterministic",
        splitChunks: {
          cacheGroups: {
            vendor: {
              test: /[\\/]node_modules[\\/]/,
              name: "vendors",
              chunks: "all",
            },
          },
        },
      }
    : {},
  output: {
    filename: "[name].[contenthash].js",
    clean: true,
  },
  devServer: {
    port: 9000,
  },
};
