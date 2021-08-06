ragel = generator(
  find_program('ragel'),
  output  : '@BASENAME@.cpp',
  arguments : ['@INPUT@', '-G2', '-e', '-o@OUTPUT@']
)

base_lib = static_library(
  'base',
  [
    'config/agents.cpp',
    'config/load.cpp',
    'config/tasks.cpp',
    'cli/progress.cpp',
    'cli/terminal.cpp',
    'cli/cli.cpp',
    'cli/events.cpp',
    'configuration.cpp',
    'exceptions.cpp',
    'predict.cpp',
    'stun/schedule.cpp',
    'stun/exp_curve_fitter.cpp',
    'stun/optimizer.cpp',
    'stun/optimizer_job.cpp',
    'stun/random_utils.cpp',
    'stun/schedule_params.cpp',
    'stun/stochastic_tunneling.cpp',
    'stun/temperature.cpp',
    'stun/utils.cpp',
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

configure_file(output : 'config.h',
  configuration : {
    'ANGONOKA_VERSION' : '"'+meson.project_name()+' version '+meson.project_version()+'"',
    'ANGONOKA_NAME' : '"'+meson.project_name()+'"',
    'ANGONOKA_OPENMP' : openmp.found()
  }
)

angonoka_exe = executable(
  meson.project_name()+'-'+target_machine.cpu_family(),
  ['main.cpp'],
  dependencies : base,
  install : true,
  install_rpath : '$ORIGIN/../lib64'
)
