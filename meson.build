project('ccs', 'cpp')
dependencies = [dependency('yaml-cpp')]

add_global_arguments('-DNOMINMAX -DSWELL_PROVIDED_BY_APP', language : 'cpp')

incdir = include_directories('WDL/swell')

shared_library(
    'reaper_ccs',
    [
      'src/reaper_slmk3.cpp',
      'src/csurf_novation_slmk3.cpp',
      'WDL/swell/swell-modstub-generic.cpp'
    ],
    dependencies: dependencies,
    include_directories: incdir
)