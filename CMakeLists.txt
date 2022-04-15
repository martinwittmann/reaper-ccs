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

  WDL/swell/swell-modstub-generic.cpp

  src/ccs/actions/Action.cpp
  src/ccs/actions/ActionInvocation.cpp
  src/ccs/actions/ActionsManager.cpp
  src/ccs/actions/ActionProvider.cpp
  src/ccs/actions/CompositeAction.cpp

  src/ccs/config/BaseConfig.cpp
  src/ccs/config/CompositeActionConfig.cpp
  src/ccs/config/FxPluginConfig.cpp
  src/ccs/config/GlobalConfig.cpp
  src/ccs/config/MappingConfig.cpp
  src/ccs/config/MidiControllerConfig.cpp
  src/ccs/config/PageConfig.cpp
  src/ccs/config/SessionConfig.cpp

  src/ccs/midi/MidiController.cpp
  src/ccs/midi/MidiControlElement.cpp
  src/ccs/midi/MidiControlElementMapping.cpp
  src/ccs/midi/MidiEventType.h
  src/ccs/midi/MidiEventSubscriber.cpp

  src/reaper/ControlSurface.cpp
  src/reaper/ThreadedMidiOutput.cpp
  src/reaper/reaper_extension.cpp

  src/reaper-api/ControlSurfaceEventSubscription.cpp
  src/reaper-api/FxParameterChangedSubscription.cpp
  src/reaper-api/FxPresetChangedSubscription.cpp
  src/reaper-api/ReaperDataTracker.cpp
  src/reaper-api/ReaperApi.cpp
  src/reaper-api/ReaperEventSubscriber.cpp

  src/ccs/Ccs.cpp
  src/ccs/CcsException.cpp
  src/ccs/Ccs_ControlSurface.cpp
  src/ccs/FxPlugins.cpp src/ccs/FxPlugins.h
  src/ccs/Page.cpp
  src/ccs/ReaperActions.cpp
  src/ccs/Session.cpp
  src/ccs/Util.cpp
  src/ccs/Variables.cpp
)

set(
  REAPER_SOURCE_FILES
  src/_old_csurf_slmk3.cpp
  WDL/swell/swell-modstub-generic.cpp
)

include_directories("/usr/local/include")
set(Boost_DEBUG 1)

find_library(YAML_CPP NAMES yaml-cpp REQUIRED)

find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIRS}")


add_library(ccs SHARED ${SOURCE_FILES})
target_include_directories(ccs PRIVATE WDL/swell)
set_target_properties(ccs PROPERTIES OUTPUT_NAME "ccs")
target_link_libraries(ccs ${YAML_CPP} stdc++fs)

#add_executable(ccs_exec src/main.cpp)
#target_link_libraries(ccs_exec ${YAML_CPP} stdc++fs)
#target_include_directories(ccs_exec PRIVATE WDL/swell)
#set_target_properties(ccs_exec PROPERTIES OUTPUT_NAME "ccs")
