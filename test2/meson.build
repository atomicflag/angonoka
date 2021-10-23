test_deps = []
foreach d : ['doctest', 'trompeloeil']
  test_deps += [
      dependency(d,
        required : get_option('tests'),
        disabler : true)
  ]
endforeach

base_test = executable(
  meson.project_name()+'_2_test',
  [
    'main.cpp',
    'configuration.cpp',
    'parse_duration.cpp',
  ],
  dependencies : [ base, test_deps ],
  cpp_args : ['-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS'],
)
test('Unit tests 2', base_test)
