project(
  'angonoka',
  'cpp',
  version : '0.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++2a']
)

compiler = meson.get_compiler('cpp')

args = compiler.get_supported_arguments([
  '-Wconversion',
  '-Wthread-safety',
  '-fasynchronous-unwind-tables',
  '-Wnon-virtual-dtor',
  '-Wshadow',
  '-Wcast-align',
  '-Wunused'
])
add_project_arguments(args, language : 'cpp')

if not get_option('debug')
  add_project_arguments(
    [
      '-Dgsl_CONFIG_CONTRACT_CHECKING_OFF',
      '-Dgsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME',
      '-ffast-math'
    ],
    language : 'cpp'
  )
else
  add_project_arguments(
    ['-Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS'],
    language : 'cpp'
  )
endif

dependencies = [
  dependency('fmt'),
  dependency('yaml-cpp'),
  dependency('clipp'),
  dependency('range-v3'),
  dependency('gsl-lite'),
  dependency('pcg-cpp'),
  dependency('boost', include_type : 'system')
]

ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

include = include_directories('src')

common_lib = static_library(
  'common',
  [
    'src/config/agents.cpp',
    'src/config/load.cpp',
    'src/config/tasks.cpp',
    'src/system.cpp',
    'src/exceptions.cpp',
    'src/stun/beta_driver.cpp',
    'src/stun/task_duration_cache.cpp',
    'src/stun/makespan_estimator.cpp',
    'src/stun/task_agents.cpp',
    'src/stun/random_utils.cpp',
    'src/stun/stochastic_tunneling.cpp',
    ragel.process('src/config/parse_duration.rl.cpp')
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
  ['src/angonoka.cpp'],
  dependencies : common,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)

test_exe = executable(
  meson.project_name()+'_test',
  [
    'test/catch.cpp',
    'test/load_agents.cpp',
    'test/stun/beta_driver.cpp',
    'test/stun/task_duration_cache.cpp',
    'test/stun/stochastic_tunneling.cpp',
    'test/stun/makespan_estimator.cpp',
    'test/stun/random_utils.cpp',
    'test/stun/task_agents.cpp',
    'test/load_tasks.cpp',
    'test/parse_duration.cpp',
    'test/system.cpp',
    'test/validation.cpp',
  ],
  dependencies : [common, dependency('catch2')],
  cpp_args : '-DANGONOKA_UNIT_TEST'
)
test('catch', test_exe)
