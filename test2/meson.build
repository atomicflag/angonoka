test_deps = []
foreach d : ['catch2', 'trompeloeil']
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
    'load_tasks.cpp',
    'load_agents.cpp',
  ],
  dependencies : [ base, test_deps ],
  cpp_args : ['-DCATCH_CONFIG_FAST_COMPILE'],
)
test('Unit tests 2', base_test)
