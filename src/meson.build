ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

stubs = files([
    'stun/stochastic_tunneling.cpp',
])

common_lib = static_library(
  'common',
  [
    'config/agents.cpp',
    'config/load.cpp',
    'config/tasks.cpp',
    'system.cpp',
    'exceptions.cpp',
    'stun/utils.cpp',
    'stun/common.cpp',
    'stun/temperature.cpp',
    'stun/random_utils.cpp',
    'stun/schedule_info.cpp',
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

angonoka_exe = executable(
  meson.project_name()+'-'+target_machine.cpu_family(),
  ['angonoka.cpp'] + stubs,
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)

stun_exe = executable(
  meson.project_name()+'-stun',
  ['stun.cpp'] + stubs,
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)
