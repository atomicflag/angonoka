test_deps = []
foreach d : ['boost-ext-ut', 'trompeloeil']
  test_deps += [
      dependency(d,
        required : get_option('tests'),
        disabler : true)
  ]
endforeach

base_test = executable(
  meson.project_name()+'_test',
  [
    'main.cpp',
    'load_agents.cpp',
    'load_tasks.cpp',
    'parse_duration.cpp',
    'configuration.cpp',
    'validation.cpp',
    'stun/common.cpp',
    'stun/exp_curve_fitter.cpp',
    'stun/makespan.cpp',
    'stun/mutate.cpp',
    'stun/optimizer.cpp'
  ],
  dependencies : [ base, test_deps ]
)
test('Unit tests', base_test)
