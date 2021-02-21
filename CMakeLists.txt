cmake_minimum_required(VERSION 3.10)

set(ENGINE_VERSION_MAJOR 0 CACHE INTERNAL "Major version of the project")
set(ENGINE_VERSION_MINOR 0 CACHE INTERNAL "Minor version of the project")
set(ENGINE_VERSION_PATCH 1 CACHE STRING "Patch version of the project")

# set the project name
project(Engine VERSION 0.1)

configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_BINARY_DIR}/config/Config.h"
  )

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")

file(GLOB_RECURSE SOURCES ${SRC_DIR} *.*)
list(FILTER SOURCES INCLUDE REGEX "${SRC_DIR}/*" )

if (WIN32)
add_executable(Engine WIN32 ${SOURCES})
else()
add_executable(${PROJECT_NAME} ${SOURCES})
endif (WIN32)


function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add Config.h to include directories
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/config")

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")


# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} glad)

# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory("${GLM_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}")

# include stb_image
target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}")

set(GTEST_DIR "${LIB_DIR}/gtest")

option(test "build all tests." ON)

if(test)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  
  add_subdirectory(${GTEST_DIR})

  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${SRC_DIR})

  # Unit Tests
  # Add test cpp file
  file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests *.*)
  list(FILTER TEST_SOURCES INCLUDE REGEX "${PROJECT_SOURCE_DIR}/tests/*" )
  add_executable(runUnitTests ${TEST_SOURCES})
  target_link_libraries(runUnitTests gtest gtest_main)
  add_test(NAME TEST COMMAND runUnitTests)

endif()