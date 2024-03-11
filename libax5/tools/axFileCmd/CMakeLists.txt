set(PROJECT_NAME axFileCmd)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "libax/tools")

target_link_libraries(${PROJECT_NAME} PUBLIC ax_core)
