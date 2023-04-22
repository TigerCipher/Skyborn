
# TODO: Make it so my python script doesn't add platform specific files here. Should be added separately in a conditinal statement
set(SOURCE_FILES src/Skyborn/Core/Application.cpp src/Skyborn/Core/Clock.cpp src/Skyborn/Core/Event.cpp src/Skyborn/Core/Input.cpp src/Skyborn/Core/PlatformWin32.cpp src/Skyborn/Debug/Logger.cpp src/Skyborn/Graphics/Renderer.cpp src/Skyborn/Graphics/Vulkan/VkCore.cpp src/Skyborn/Graphics/Vulkan/VkImage.cpp src/Skyborn/Graphics/Vulkan/VkInterface.cpp src/Skyborn/Graphics/Vulkan/VkSwapchain.cpp )


add_library(skyborn SHARED ${SOURCE_FILES})
target_include_directories(skyborn PRIVATE src $ENV{VULKAN_SDK}/Include)
target_link_directories(skyborn PUBLIC $ENV{VULKAN_SDK}/Lib)
target_link_libraries(skyborn vulkan-1)
# add_compile_definitions(_DEBUG SKY_EXPORT _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(skyborn PRIVATE _DEBUG SKY_EXPORT _CRT_SECURE_NO_WARNINGS)