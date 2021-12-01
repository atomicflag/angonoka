/* eslint-disable */

// https://github.com/pmmmwh/react-refresh-webpack-plugin/issues/176#issuecomment-686536169

if (module.hot) {
  global.$RefreshReg$ = () => {};
  global.$RefreshSig$ = () => () => {};
}
