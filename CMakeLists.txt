cmake_minimum_required(VERSION 3.18)
project(ccs)
set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(-DSWELL_PROVIDED_BY_APP -DNOMINMAX)

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
  src/reaper_slmk3.cpp
  src/csurf_novation_slmk3.cpp
  WDL/swell/swell-modstub-generic.cpp
)
include_directories("/usr/local/include")

find_library(
  MYYAML
  NAMES yaml-cpp
  REQUIRED
)

if (NOT ${MYYAML})
  message(STATUS "huih")
else()
  message(FATAL_ERROR "NOT FOUND!!!!!")
endif()

add_library(ccs SHARED ${SOURCE_FILES})

target_include_directories(ccs PRIVATE WDL/swell)
target_link_libraries(${PROJECT_NAME} ${MYYAML} stdc++fs)

#target_link_libraries(ccs /home/martin/audio/yaml-cpp/libyaml-cpp.so.0.7.0)

set_target_properties(ccs PROPERTIES OUTPUT_NAME "ccs")

