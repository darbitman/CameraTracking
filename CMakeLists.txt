cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

project(CameraTracking CXX)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)



# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in ${CMAKE_SOURCE_DIR}/googletest-download/CMakeLists.txt)

# Generate CMakeLists cache/files for googletest download
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

# Use the generated CMakeLists files to "build" (download) googletest
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "")
set(INSTALL_GTEST OFF CACHE BOOL "")
set(BUILD_GMOCK OFF CACHE BOOL "")
set(DEBUGDISPLAY OFF CACHE BOOL "")

if(${DEBUGDISPLAY})
  add_definitions(-DUSEDEBUGDISPLAY)
endif()
#get_directory_property(DirDefs COMPILE_DEFINITIONS)
#message("COMPILE_DEFINITIONS = ${DirDefs}")

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_SOURCE_DIR}/googletest
                 ${CMAKE_SOURCE_DIR}/googletest-build
                 )
                 




# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("include/DebugDisplay")
add_subdirectory("src")
