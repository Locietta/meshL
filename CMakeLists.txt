cmake_minimum_required(VERSION 3.21.0)

# uncomment this if you're using `vcpkg` for MSVC
# 
# set(CMAKE_TOOLCHAIN_FILE D:/vcpkg/scripts/buildsystems/vcpkg.cmake 
#   CACHE STRING "Vcpkg toolchain file")

project(meshL VERSION 0.1.0)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17 REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_library(mesh STATIC mesh.cpp material.cpp)
target_include_directories(
    mesh 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE   ${CMAKE_CURRENT_SOURCE_DIR}/include ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(
  mesh 
  INTERFACE ${OpenCV_LIBRARIES}
  PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT
)
target_precompile_headers(mesh PUBLIC ${PROJECT_SOURCE_DIR}/pch/pch.h)

file(GLOB ASSET "assets/*")

add_executable(test_obj test_obj.cpp)
target_link_libraries(test_obj mesh)
add_custom_command(TARGET test_obj POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test_obj> $<TARGET_FILE_DIR:test_obj>
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:test_obj>/../assets # in case there's no assets served
  COMMAND ${CMAKE_COMMAND} -E copy ${ASSET} $<TARGET_FILE_DIR:test_obj>/../assets
  COMMAND_EXPAND_LISTS
)