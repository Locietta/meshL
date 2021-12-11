cmake_minimum_required(VERSION 3.21.0)

# if you're using `vcpkg` to resolve dependencies
# please set `toolchainFile` to path-to-vcpkg\\scripts\\buildsystems\\vcpkg.cmake for presets in your cmake-tools-kits.json

project(meshL VERSION 0.1.0)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

if ((NOT DEFINED CMAKE_CXX_STANDARD) OR (CMAKE_CXX_STANDARD LESS 17))
  set(CMAKE_CXX_STANDARD 17 REQUIRED)
endif()

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

add_executable(test_obj test_obj.cpp)
target_link_libraries(test_obj mesh)
add_custom_command(TARGET test_obj POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test_obj> $<TARGET_FILE_DIR:test_obj>
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:test_obj>/../assets
  COMMAND_EXPAND_LISTS
)