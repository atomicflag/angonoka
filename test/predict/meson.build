# TODO: Can we get rid of relative paths?
predict_test = executable(
  'predict_test',
  [
    'main.cpp',
    'predict.cpp',
    '../../src/configuration.cpp',
    '../../src/predict.cpp',
  ],
  dependencies : [ test_deps, dependencies ],
  include_directories : ['.', include],
  cpp_args: ['-DUNIT_TEST']
)
test('Predict unit test', predict_test)
