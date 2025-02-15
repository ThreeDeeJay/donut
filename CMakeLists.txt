cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# make sure all submodules are pulled
execute_process(COMMAND git submodule update --init --recursive
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Default to vcpkg toolchain file")
endif()

# Remove MinSizeRel build configuration, not useful
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE STRING INTERNAL FORCE )

project(donut)

# Configure C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# disable in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# default build type to RelWithDebInfo
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# Output binaries to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Check if our compiler has filesystem or experimental/filesystem
include(CheckIncludeFileCXX)
check_include_file_cxx("filesystem" _CXX_FILESYSTEM_HAVE_HEADER)
check_include_file_cxx("experimental/filesystem" _CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER)
mark_as_advanced(_CXX_FILESYSTEM_HAVE_HEADER)
mark_as_advanced(_CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER)

# Dependencies
find_package(SDL2 CONFIG REQUIRED)
find_package(libpng CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

# Wrap OpenAL Soft in an interface too
if (OPENAL_FOUND AND NOT TARGET OpenAL::OpenAL)
  add_library(OpenAL::OpenAL INTERFACE IMPORTED GLOBAL)
  set_target_properties(OpenAL::OpenAL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR})
  set_target_properties(OpenAL::OpenAL PROPERTIES INTERFACE_LINK_LIBRARIES ${OPENAL_LIBRARY})
endif()

# Setup a custom clang-format target (if clang-format can be found)
find_program(CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
  set(SRCS ${PROJECT_SOURCE_DIR}/src)
  set(CCOMMENT "Running clang format against all the .h and .cpp files in src/")
  if (WIN32)
      add_custom_target(clang-format
          COMMAND powershell.exe -Command "Get-ChildItem '${SRCS}/*' -Include *.cpp,*.h -Recurse | Foreach {&'${CLANG_FORMAT}' -i $_.fullname}"
          COMMENT ${CCOMMENT})
  elseif(MINGW)
      add_custom_target(clang-format
          COMMAND find `cygpath -u ${SRCS}` -iname *.h -o -iname *.cpp | xargs `cygpath -u ${CLANG_FORMAT}` -i
          COMMENT ${CCOMMENT})
  else()
      add_custom_target(clang-format
          COMMAND find ${SRCS} -iname *.h -o -iname *.cpp | xargs ${CLANG_FORMAT} -i
          COMMENT ${CCOMMENT})
  endif()
  unset(SRCS)
  unset(CCOMMENT)
endif()

# The Code
add_subdirectory(src)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/bin/$<CONFIG>")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
