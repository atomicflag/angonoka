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
    'validation.cpp',
    'cli/duration.cpp',
    'cli/json_schedule.cpp',
    'stun/schedule.cpp',
    'stun/exp_curve_fitter.cpp',
    'stun/makespan.cpp',
    'stun/mutate.cpp',
    'stun/optimizer.cpp',
    'stun/optimizer_job.cpp',
    'stun/schedule_params.cpp',
    'stun/temperature.cpp'
  ],
  dependencies : [ base, test_deps ]
)
test('Unit tests', base_test)

subdir('stun/stochastic_tunneling')
subdir('predict')
