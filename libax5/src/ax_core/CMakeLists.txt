set(PROJECT_NAME ax_core)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_folder_all_h(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "libax")

target_link_libraries(${PROJECT_NAME} PUBLIC ax_detect_platform)

find_package(Tracy REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Tracy::TracyClient)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PUBLIC -luuid)
	target_link_libraries(${PROJECT_NAME} PUBLIC -lX11 -lXt -lxcb -lXdmcp -lXau -lXext -lGL)
endif()