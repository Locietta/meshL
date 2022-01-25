cmake_minimum_required(VERSION 3.21.0)

# if you're using `vcpkg` to resolve dependencies
# please set `toolchainFile` to path-to-vcpkg\\scripts\\buildsystems\\vcpkg.cmake for presets in your cmake-tools-kits.json
project(meshL VERSION 0.1.0)

find_package(OpenGL REQUIRED)

add_library(mesh STATIC mesh.cpp)
target_include_directories(
  mesh
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(
  mesh
  PRIVATE OpenGL::GL OpenGL::GLU
)
target_precompile_headers(mesh PUBLIC ${meshL_SOURCE_DIR}/pch/pch.h)
target_compile_features(mesh PUBLIC cxx_std_17)

add_subdirectory(test)
