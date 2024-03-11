set(PROJECT_NAME ax_core_unit_test)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_test_all_cpp(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "libax")
target_link_libraries(${PROJECT_NAME} PUBLIC ax_core)