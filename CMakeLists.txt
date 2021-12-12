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

find_package(OpenCV CONFIG REQUIRED)
add_executable(test_obj test_obj.cpp)
target_include_directories(test_obj PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_obj PRIVATE mesh ${OpenCV_LIBRARIES})
add_custom_command(TARGET test_obj POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test_obj> $<TARGET_FILE_DIR:test_obj>
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/examples $<TARGET_FILE_DIR:test_obj>/../examples
  COMMAND_EXPAND_LISTS
)