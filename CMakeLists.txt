cmake_minimum_required(VERSION 3.13)
project(reaper_slmk3)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(-DSWELL_PROVIDED_BY_APP -DNOMINMAX)

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
  src/reaper_slmk3.cpp
  src/csurf_novation_slmk3.cpp
  WDL/swell/swell-modstub-generic.cpp
)

add_library(reaper_slmk3 SHARED ${SOURCE_FILES})
target_include_directories(reaper_slmk3 PRIVATE WDL/swell)

set_target_properties(reaper_slmk3 PROPERTIES OUTPUT_NAME "reaper_slmk3")