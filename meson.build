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
    ] + compiler.get_supported_arguments(['-ffast-math']),
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
  dependency('indicators'),
  dependency('boost', include_type : 'system')
]

include = include_directories('src')

subdir('src')
subdir('test')
