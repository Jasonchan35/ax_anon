set(PROJECT_NAME ax_render)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_folder_all_h(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libax")

target_link_libraries(${PROJECT_NAME} PUBLIC ax_core)

if(WIN32)
	set(vulkan_sdk_path "C:/VulkanSDK/1.2.198.1")
	target_include_directories(${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Include")
	target_link_directories(   ${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Lib")
	target_link_libraries(${PROJECT_NAME} PUBLIC vulkan-1)
endif()

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(mozjpeg REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC mozjpeg::jpeg mozjpeg::turbojpeg)

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype)


