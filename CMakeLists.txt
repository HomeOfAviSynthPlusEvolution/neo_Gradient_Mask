cmake_minimum_required (VERSION 3.15)
project (neo_gradient_mask CXX)
file(GLOB CODE "src/*.cpp")
add_library(neo-gradient-mask SHARED main.cpp ${CODE})
set_property(TARGET neo-gradient-mask PROPERTY CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

pkg_check_modules(AVISYNTH avisynth)
if(AVISYNTH_FOUND)
  include_directories(${AVISYNTH_INCLUDE_DIRS})
else()
  include_directories(include/avisynth)
endif()

pkg_check_modules(VAPOURSYNTH vapoursynth)
if(VAPOURSYNTH_FOUND)
  include_directories(${VAPOURSYNTH_INCLUDE_DIRS})
else()
  include_directories(include/vapoursynth)
endif()

include_directories(.)
include_directories(include/dualsynth)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if (CMAKE_GENERATOR_TOOLSET MATCHES "v[0-9]*_xp")
    target_compile_definitions(neo-gradient-mask PRIVATE WINVER=0x502 _WIN32_WINNT=0x502)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:threadSafeInit-")
  endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(neo-gradient-mask tbb)

endif()

add_custom_command(
  TARGET neo-gradient-mask POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:neo-gradient-mask> "../Release/${_DIR}/$<TARGET_FILE_NAME:neo-gradient-mask>"
)
