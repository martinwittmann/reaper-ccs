cmake_minimum_required(VERSION 3.18)
project(ccs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_definitions(-DSWELL_PROVIDED_BY_APP -DNOMINMAX)

set(
  SOURCE_FILES
  src/reaper_slmk3.cpp
  src/csurf_novation_slmk3.cpp

  src/config/BaseConfig.cpp
  src/config/PageConfig.cpp
  src/config/SessionConfig.cpp
  src/config/MappingConfig.cpp
  src/config/MidiControllerConfig.cpp
  src/config/GlobalConfig.cpp

  src/actions/Action.cpp
  src/actions/ActionsManager.cpp
  src/actions/ActionProvider.cpp
  src/actions/ActionInvocation.cpp

  src/reaper/ReaperDataTracker.cpp
  src/reaper/ReaperApi.cpp
  src/reaper/ReaperEventSubscriber.cpp

  src/Ccs.cpp
  src/MidiController.cpp
  src/MidiControlElement.cpp
  src/MidiControlElementMapping.cpp
  src/MidiEventType.cpp
  src/MidiEventSubscriber.cpp
  src/Page.cpp
  src/Session.cpp
  src/Util.cpp
  src/Variables.cpp

  WDL/swell/swell-modstub-generic.cpp
)

set(
  REAPER_SOURCE_FILES
  src/csurf_novation_slmk3.cpp
  WDL/swell/swell-modstub-generic.cpp
)

include_directories("/usr/local/include")
set(Boost_DEBUG 1)

find_library(YAML_CPP NAMES yaml-cpp REQUIRED)

find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIRS}")


#add_library(ccs SHARED ${SOURCE_FILES})
#target_include_directories(ccs PRIVATE WDL/swell)
#set_target_properties(ccs PROPERTIES OUTPUT_NAME "ccs")
#target_link_libraries(ccs ${YAML_CPP} stdc++fs)

add_executable(ccs_exec src/main.cpp ${SOURCE_FILES})
target_link_libraries(ccs_exec ${YAML_CPP} stdc++fs)
target_include_directories(ccs_exec PRIVATE WDL/swell)
set_target_properties(ccs_exec PROPERTIES OUTPUT_NAME "ccs")
