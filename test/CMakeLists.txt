project(test_objloader)

find_package(GLUT REQUIRED)
add_executable(test_obj test_obj.cpp obj_loader.cpp)
target_include_directories(test_obj
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(test_obj PRIVATE mesh OpenGL::GL OpenGL::GLU GLUT::GLUT)
add_custom_command(TARGET test_obj POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test_obj> $<TARGET_FILE:test_obj> $<TARGET_FILE_DIR:test_obj>
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${test_objloader_SOURCE_DIR}/../assets $<TARGET_FILE_DIR:test_obj>/../assets
  COMMAND_EXPAND_LISTS
)