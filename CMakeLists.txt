cmake_minimum_required(VERSION 3.10)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(ENGINE_VERSION_MAJOR 0 CACHE INTERNAL "Major version of the project")
set(ENGINE_VERSION_MINOR 0 CACHE INTERNAL "Minor version of the project")
set(ENGINE_VERSION_PATCH 1-alpha CACHE STRING "Patch version of the project")


project(Engine)

configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_BINARY_DIR}/config/Config.h"
  )

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")

file(GLOB_RECURSE CLIENT_SOURCES ${SRC_DIR} *.cpp)
file(GLOB_RECURSE CLIENT_HEADERS ${SRC_DIR} *.h)
file(GLOB_RECURSE SERVER_SOURCES ${SRC_DIR} *.cpp)
file(GLOB_RECURSE SERVER_HEADERS ${SRC_DIR} *.h)
file(GLOB_RECURSE LIB_SOURCES ${SRC_DIR} *.cpp)
file(GLOB_RECURSE LIB_HEADERS ${SRC_DIR} *.h)

# separate sources by server/client/core

list(FILTER LIB_SOURCES INCLUDE REGEX "${SRC_DIR}/*" )
list(FILTER LIB_HEADERS INCLUDE REGEX "${SRC_DIR}/*")
list(FILTER LIB_SOURCES EXCLUDE REGEX "Client.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX "Server.cpp")

list(FILTER CLIENT_SOURCES INCLUDE REGEX "${SRC_DIR}/*" )
list(FILTER CLIENT_HEADERS INCLUDE REGEX "${SRC_DIR}/*")
list(FILTER CLIENT_SOURCES EXCLUDE REGEX "Server.cpp")

list(FILTER SERVER_SOURCES INCLUDE REGEX "${SRC_DIR}/*" )
list(FILTER SERVER_HEADERS INCLUDE REGEX "${SRC_DIR}/*")
list(FILTER SERVER_SOURCES EXCLUDE REGEX "Client.cpp")
list(FILTER SERVER_SOURCES EXCLUDE REGEX "/client/")
list(FILTER SERVER_HEADERS EXCLUDE REGEX "/client/")

add_library( Engine ${LIB_SOURCES} ${LIB_HEADERS} )
if (WIN32)
add_executable(Client WIN32 ${CLIENT_SOURCES} ${CLIENT_HEADERS})
add_executable(Server WIN32 ${SERVER_SOURCES} ${SERVER_HEADERS})
else()
add_executable(Client ${CLIENT_SOURCES} ${CLIENT_HEADERS})
add_executable(Server ${SERVER_SOURCES} ${SERVER_HEADERS})
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

assign_source_group(${LIB_SOURCES})
assign_source_group(${LIB_HEADERS})
assign_source_group(${CLIENT_SOURCES})
assign_source_group(${CLIENT_HEADERS})
assign_source_group(${SERVER_SOURCES})
assign_source_group(${SERVER_HEADERS})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(Client PRIVATE "${SRC_DIR}")
target_include_directories(Server PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET Client PROPERTY CXX_STANDARD 17)
set_property(TARGET Server PROPERTY CXX_STANDARD 17)

# add Config.h to include directories
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/config")
target_include_directories(Client PRIVATE "${PROJECT_BINARY_DIR}/config")
target_include_directories(Server PRIVATE "${PROJECT_BINARY_DIR}/config")


# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(Client "glfw" "${GLFW_LIBRARIES}")
target_include_directories(Client PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(Client PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(Client PRIVATE "${GLAD_DIR}/include")
target_link_libraries(Client glad)

# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory("${GLM_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}")
target_include_directories(Client PRIVATE "${GLM_DIR}")
target_include_directories(Server PRIVATE "${GLM_DIR}")

# include stb_image
target_include_directories(Client PRIVATE "${LIB_DIR}")

set(GTEST_DIR "${LIB_DIR}/gtest")

option(tests "build all tests." ON)

if(tests)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  
  add_subdirectory(${GTEST_DIR})

  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${SRC_DIR})

  # Unit Tests
  # Add test cpp file
  file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests *.*)
  list(FILTER TEST_SOURCES INCLUDE REGEX "${PROJECT_SOURCE_DIR}/tests/*" )
  add_executable(Tests ${TEST_SOURCES})
  target_link_libraries(Tests gtest gtest_main)
  add_test(NAME TEST COMMAND runUnitTests)

endif()

# move libraries to special folder
set_target_properties(glfw PROPERTIES FOLDER Libraries)
set_target_properties(glad PROPERTIES FOLDER Libraries)
set_target_properties(gmock PROPERTIES FOLDER Libraries)
set_target_properties(gmock_main PROPERTIES FOLDER Libraries)
set_target_properties(gtest PROPERTIES FOLDER Libraries)
set_target_properties(gtest_main PROPERTIES FOLDER Libraries)

# move projects to Engine folder
set_target_properties(Engine PROPERTIES FOLDER Engine)
set_target_properties(Client PROPERTIES FOLDER Engine)
set_target_properties(Server PROPERTIES FOLDER Engine)
set_target_properties(Tests  PROPERTIES FOLDER Engine)