project('angonoka', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++2a'])

compiler = meson.get_compiler('cpp')

if compiler.has_argument('-Wconversion')
	add_project_arguments(['-Wconversion'], language : 'cpp')
endif

dependencies = [
	dependency('fmt'),
	dependency('yaml-cpp'),
	dependency('clipp'),
	dependency('range-v3'),
	dependency('boost')
]

sources = [
	'src/angonoka.cpp',
]

exe = executable(
	meson.project_name()+'-'+target_machine.cpu_family(),
	sources,
	dependencies: dependencies,
	install : true)

test_sources = [
	'test/catch.cpp',
	'test/load_agents.cpp',
	'test/load_tasks.cpp',
	'src/config/load.cpp',
	'src/config/agents.cpp',
	'src/config/tasks.cpp'
]

test_exe = executable(meson.project_name()+'_test',
	test_sources,
	include_directories: include_directories('src'),
	dependencies: dependencies+[ dependency('Catch2') ])
test('catch', test_exe)
