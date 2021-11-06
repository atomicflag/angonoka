const path = require('path');

const isProductionMode = process.env.NODE_ENV === "production";

const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = {
  entry: './src/index.ts',
  devtool: 'inline-source-map',
  mode: isProductionMode ? "production" : "development",
  module: {
    rules: [{
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
      {
        test: /\.css$/i,
        use: [
          isProductionMode ? MiniCssExtractPlugin.loader : "style-loader",
          "css-loader",
          "postcss-loader"
        ],
      },
    ],
  },
  resolve: {
    extensions: ['.tsx', '.ts', '.js'],
  },
  plugins: [
    new MiniCssExtractPlugin({
      filename: isProductionMode ? "[name].[contenthash].css" : "[name].css",
    }),
    new HtmlWebpackPlugin({
      title: "Schedule Visualizer"
    })
  ],
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, 'dist'),
  },
};