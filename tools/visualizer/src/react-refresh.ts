/* eslint-disable */

// https://github.com/pmmmwh/react-refresh-webpack-plugin/issues/176#issuecomment-686536169

if (module.hot) {
  // @ts-expect-error see above
  global.$RefreshReg$ = () => {};
  // @ts-expect-error see above
  global.$RefreshSig$ = () => () => {};
}
