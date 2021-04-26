# TODO: Can we get rid of relative paths?
predict_test = executable(
  'predict_test',
  [
    'main.cpp',
    'predict.cpp',
    '../../src/stun/schedule_params.cpp',
    '../../src/configuration.cpp',
  ],
  dependencies : [ test_deps, dependencies ],
  include_directories : ['.', include]
)
test('Predict unit test', stochastic_tunneling_test)
