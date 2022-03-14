cmake_minimum_required(VERSION 3.13)
project(reaper_slmk3)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
  src/reaper_slmk3.cpp
  src/csurf_novation_slmk3.cpp
  WDL/swell/swell-functions.h
  WDL/swell/swell-internal.h
  WDL/swell/swell-dlggen.h
  WDL/swell/swell-gdi-internalpool.h
  WDL/swell/swell-menugen.h
  WDL/swell/swell-types.h
)

#set(HEADER_FILES
  #swell-functions.h
  #swell-internal.h
  #swell-dlggen.h
  #swell-gdi-internalpool.h
  #swell-menugen.h
  #swell-types.h
#)



add_definitions(-DSWELL_PROVIDED_BY_APP -DNOMINMAX)
add_definitions(
  -include ../WDL/swell/swell-types.h
  -include ../WDL/swell/swell-functions.h
  -include ../WDL/swell/swell-internal.h
  -include ../WDL/swell/swell-dlggen.h
  -include ../WDL/swell/swell-gdi-internalpool.h
  -include ../WDL/swell/swell-menugen.h
  -include ../WDL/swell/swell-types.h
)

add_library(reaper_slmk3 ${SOURCE_FILES})
target_include_directories(reaper_slmk3 PRIVATE WDL/swell)

set_target_properties(reaper_slmk3 PROPERTIES OUTPUT_NAME "reaper_slmk3.so")