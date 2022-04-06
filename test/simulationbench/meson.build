root_dir = '../../'

nanobench = dependency('nanobench',
      required : get_option('benchmarks'),
      disabler : true)

simulation_benchmark = executable(
  'simulation_benchmark',
  [ 'simulation.cpp' ],
  link_with : base_lib,
  dependencies : [ dependencies, nanobench ],
  include_directories : ['.', include],
)
