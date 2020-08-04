project('angonoka', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++2a']
)

compiler = meson.get_compiler('cpp')

if compiler.has_argument('-Wconversion')
  add_project_arguments(['-Wconversion'], language : 'cpp')
endif

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
  dependency('pybind11'),
  dependency('pcg-cpp'),
  dependency('boost', include_type: 'system')
]

ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

include = include_directories('src')
sources = [
  'src/config/agents.cpp',
  'src/config/load.cpp',
  'src/config/tasks.cpp',
  'src/system.cpp',
  'src/schedule.cpp',
  'src/exceptions.cpp',
  ragel.process('src/config/parse_duration.rl.cpp')
]

exe = executable(
  meson.project_name()+'-'+target_machine.cpu_family(),
  sources + ['src/angonoka.cpp'],
  include_directories: include,
  dependencies: dependencies,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)

py_sources = [
  'src/py/module.cpp'
]

py_lib = library(
  meson.project_name(),
  py_sources,
  include_directories: include,
  dependencies: dependencies+[
    dependency('python-3.8-embed'),
    dependency('openmp', language:'cpp')
  ],
)

test_sources = [
  'test/catch.cpp',
  'test/load_agents.cpp',
  'test/load_tasks.cpp',
  'test/parse_duration.cpp',
  'test/system.cpp',
  'test/validation.cpp',
  'test/schedule.cpp'
]

test_exe = executable(
  meson.project_name()+'_test',
  sources + test_sources,
  include_directories: include,
  dependencies: dependencies + [dependency('catch2')]
)
test('catch', test_exe)
