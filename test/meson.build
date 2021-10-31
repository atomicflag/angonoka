test_deps = []
foreach d : ['boost-ext-ut', 'trompeloeil']
  test_deps += [
      dependency(d,
        required : get_option('tests'),
        disabler : true)
  ]
endforeach

subdir('predict')
