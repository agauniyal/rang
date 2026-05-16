project('rang_test', 'cpp', version : '3.2.0',
		default_options : ['cpp_std=c++11'])

rang_dep = dependency('rang')

visualTest = executable('visualTest', 'visualTest.cpp', dependencies : rang_dep)
test('visualTest', visualTest)
