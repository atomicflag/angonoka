ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

common_lib = static_library(
  'common',
  [
    'config/agents.cpp',
    'config/load.cpp',
    'config/tasks.cpp',
    'system.cpp',
    'exceptions.cpp',
    'stun/beta_driver.cpp',
    'stun/task_duration_cache.cpp',
    'stun/makespan_estimator.cpp',
    'stun/task_agents.cpp',
    'stun/random_utils.cpp',
    'stun/stochastic_tunneling.cpp',
    ragel.process('config/parse_duration.rl.cpp')
  ],
  include_directories : include,
  dependencies : dependencies,
)

common = declare_dependency(
  link_with : common_lib,
  dependencies : dependencies,
  include_directories : include
)

exe = executable(
  meson.project_name()+'-'+target_machine.cpu_family(),
  ['angonoka.cpp'],
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)
