set(PROJECT_NAME ax_ui)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_folder_all_h(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "libax")

target_link_libraries(${PROJECT_NAME} PUBLIC ax_render)
