# TODO: Can we get rid of relative paths?
stochastic_tunneling_test = executable(
  'stochastic_tunneling_test',
  [
    'main.cpp',
    'stochastic_tunneling.cpp',
    '../../../src/stun/stochastic_tunneling.cpp',
    '../../../src/stun/common.cpp',
  ],
  dependencies : [ test_deps, dependencies ],
  include_directories : ['.', include],
  cpp_args: ['-DUNIT_TEST']
)
test('Stochastic tunneling unit test', stochastic_tunneling_test)
