ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

base_lib = static_library(
  'base',
  [
    'config/agents.cpp',
    'stun/stochastic_tunneling.cpp',
    'config/load.cpp',
    'config/tasks.cpp',
    'configuration.cpp',
    'exceptions.cpp',
    'stun/utils.cpp',
    'stun/common.cpp',
    'stun/temperature.cpp',
    'stun/random_utils.cpp',
    'stun/exp_curve_fitter.cpp',
    'stun/schedule_params.cpp',
    'stun/optimizer.cpp',
    ragel.process('config/parse_duration.rl.cpp')
  ],
  include_directories : include,
  dependencies : dependencies,
)

base = declare_dependency(
  link_with : base_lib,
  dependencies : dependencies,
  include_directories : include
)

angonoka_exe = executable(
  meson.project_name()+'-'+target_machine.cpu_family(),
  ['angonoka.cpp'],
  dependencies : base,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)
