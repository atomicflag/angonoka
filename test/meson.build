test_exe = executable(
  meson.project_name()+'_test',
  [
    'catch.cpp',
    'load_agents.cpp',
    'stun/beta_driver.cpp',
    'stun/task_duration_cache.cpp',
    'stun/stochastic_tunneling.cpp',
    'stun/makespan_estimator.cpp',
    'stun/random_utils.cpp',
    'stun/task_agents.cpp',
    'load_tasks.cpp',
    'parse_duration.cpp',
    'system.cpp',
    'validation.cpp',
  ],
  dependencies : [common, dependency('catch2')]
)
test('catch', test_exe)
