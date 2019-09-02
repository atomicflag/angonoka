project('angonoka', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++2a'])

sources = [
	'src/angonoka.cpp'
]

dependencies = [
	dependency('fmt'),
	dependency('yaml-cpp'),
	dependency('clipp'),
	dependency('type_safe')
]

exe = executable(meson.project_name(),
	sources,
	dependencies: dependencies,
	install : true)

test_sources = [
	'test/catch.cpp'
]

test_exe = executable(meson.project_name()+'_test',
	test_sources,
	dependencies: dependencies+[dependency('Catch2')])
test('catch', test_exe)
