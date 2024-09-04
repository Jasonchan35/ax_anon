set(PROJECT_NAME ax_render)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_folder_all_h(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libax")

target_link_libraries(${PROJECT_NAME} PUBLIC ax_core)

find_package(mozjpeg REQUIRED)

if(WIN32)
	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
	target_include_directories(${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Include")
	target_link_directories(   ${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Lib")
	target_link_libraries(${PROJECT_NAME} PUBLIC vulkan-1)
	target_link_libraries(${PROJECT_NAME} PUBLIC mozjpeg::jpeg)
else()
	target_link_libraries(${PROJECT_NAME} PUBLIC mozjpeg::jpeg-static)
endif()

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype)


