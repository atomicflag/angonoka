project(
  'angonoka',
  'cpp',
  version : '0.5.1',
  default_options : ['warning_level=3', 'cpp_std=c++20']
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
    ] + compiler.get_supported_arguments(['-ffast-math']),
    language : 'cpp'
  )
else
  add_project_arguments(
    ['-Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS'],
    language : 'cpp'
  )
endif

openmp = dependency('openmp', required : get_option('openmp'))

dependencies = [
  dependency('fmt'),
  dependency('yaml-cpp'),
  dependency('cli11'),
  dependency('range-v3'),
  dependency('gsl-lite'),
  dependency('pcg-cpp'),
  dependency('indicators'),
  dependency('readerwriterqueue'),
  dependency('boost', include_type : 'system'),
  openmp
]

include = include_directories('src')

subdir('src')
subdir('test')
