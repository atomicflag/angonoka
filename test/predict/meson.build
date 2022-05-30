root_dir = '../../'

predict_test = executable(
  'predict_test',
  [
    'main.cpp',
    'predict.cpp',
    root_dir / 'src/project.cpp',
    root_dir / 'src/predict.cpp',
    root_dir / 'src/stun/schedule.cpp',
  ],
  dependencies : [ test_deps, dependencies ],
  include_directories : ['.', include],
  cpp_args: ['-DUNIT_TEST']
)
test('Predict unit test', predict_test)
