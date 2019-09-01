project('angonoka', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++2a'])

sources = ['angonoka.cpp']

dependencies = [
	dependency('fmt'),
	dependency('yaml-cpp'),
	dependency('clipp'),
	dependency('type_safe')
]

exe = executable('angonoka',
	sources,
	dependencies: dependencies,
	install : true)
