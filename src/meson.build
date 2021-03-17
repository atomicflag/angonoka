ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

# We need to compile 2 versions of the same TUs:
# One with abstract interfaces for unit testing
# and one with concrete classes for integration
# testing. TUs are guarded with inline namespaces
# to avoid ODR violations.
# When UNIT_TEST macro is defined TUs use
# abstract interfaces.
stubs = files([
    'stun/stochastic_tunneling.cpp',
])

common_lib = static_library(
  'common',
  [
    'config/agents.cpp',
    'config/load.cpp',
    'config/tasks.cpp',
    'configuration.cpp',
    'exceptions.cpp',
    'stun/utils.cpp',
    'stun/common.cpp',
    'stun/temperature.cpp',
    'stun/random_utils.cpp',
    'stun/schedule_params.cpp',
    'stun/optimizer.cpp',
    ragel.process('config/parse_duration.rl.cpp')
  ] + stubs,
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
  ['angonoka.cpp'],
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)

stun_exe = executable(
  meson.project_name()+'-stun',
  ['stun.cpp', 'run.cpp'],
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)
