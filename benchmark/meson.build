benchmark_exe = executable(
  meson.project_name()+'_benchmark',
  [
    'celero.cpp',
    'stochastic_tunneling.cpp',
  ] + stubs,
  dependencies : [
    common,
    dependency('Celero', required : get_option('benchmark'), disabler : true),
  ]
)
benchmark('celero', benchmark_exe)
