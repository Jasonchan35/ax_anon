set(PROJECT_NAME libAnonEditor)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_folder_all_h(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "anon")

target_link_libraries(${PROJECT_NAME} PUBLIC libAnon)
