test_deps = []
foreach d : ['catch2', 'trompeloeil']
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
    'configuration.cpp',
    'parse_duration.cpp',
    'load_tasks.cpp',
    'load_agents.cpp',
    'validation.cpp',
    'stun/exp_curve_fitter.cpp',
    'stun/makespan.cpp',
    'stun/mutate.cpp',
    'stun/optimizer_job.cpp',
    'stun/schedule_params.cpp',
    'stun/schedule.cpp',
    'stun/optimizer.cpp',
    'cli/duration.cpp',
    'cli/json_schedule.cpp',
    'stun/temperature.cpp'
  ],
  dependencies : [ base, test_deps ],
  cpp_args : ['-DCATCH_CONFIG_FAST_COMPILE'],
)
test('Unit tests', base_test)

subdir('stun/stochastic_tunneling')
subdir('predict')
