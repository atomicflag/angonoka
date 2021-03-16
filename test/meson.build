test_deps = []
foreach d : ['catch2', 'trompeloeil']
  test_deps += [
      dependency(d,
        required : get_option('tests'),
        disabler : true)
  ]
endforeach

test_exe = executable(
  meson.project_name()+'_test',
  [
    'catch.cpp',
    'load_agents.cpp',
    'stun/makespan.cpp',
    'stun/mutate.cpp',
    'stun/common.cpp',
    'stun/optimizer.cpp',
    'stun/temperature.cpp',
    'stun/schedule_params.cpp',
    'stun/stochastic_tunneling.cpp',
    'load_tasks.cpp',
    'parse_duration.cpp',
    'system.cpp',
    'validation.cpp',
  ] + stubs,
  dependencies : [
    common,
    test_deps
  ],
  cpp_args: ['-DUNIT_TEST']
)
test('catch', test_exe)
