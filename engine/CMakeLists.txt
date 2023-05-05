
# TODO: Make it so my python script doesn't add platform specific files here. Should be added separately in a conditinal statement
set(SOURCE_FILES src/Skyborn/Core/Application.cpp src/Skyborn/Core/Clock.cpp src/Skyborn/Core/Event.cpp src/Skyborn/Core/Input.cpp src/Skyborn/Debug/Logger.cpp src/Skyborn/Graphics/Renderer.cpp src/Skyborn/Graphics/Vulkan/VkCommandBuffer.cpp src/Skyborn/Graphics/Vulkan/VkCore.cpp src/Skyborn/Graphics/Vulkan/VkImage.cpp src/Skyborn/Graphics/Vulkan/VkInterface.cpp src/Skyborn/Graphics/Vulkan/VkRenderpass.cpp src/Skyborn/Graphics/Vulkan/VkSurface.cpp src/Skyborn/Graphics/Vulkan/VkSwapchain.cpp src/Skyborn/Graphics/Vulkan/VkFence.cpp src/Skyborn/Graphics/Vulkan/VkFramebuffer.cpp   "src/Skyborn/Graphics/Vulkan/VkHelpers.h")

if(WIN32)
    set(SOURCE_FILES ${SOURCE_FILES} src/Skyborn/Core/PlatformWin32.cpp)
endif()

add_library(skyborn SHARED ${SOURCE_FILES} "src/Skyborn/Graphics/Vulkan/VkHelpers.cpp" "src/Skyborn/Util/FileSystem.h" "src/Skyborn/Util/FileSystem.cpp")
target_include_directories(skyborn PRIVATE src $ENV{VULKAN_SDK}/Include)
target_link_directories(skyborn PUBLIC $ENV{VULKAN_SDK}/Lib)
target_link_libraries(skyborn vulkan-1)
# add_compile_definitions(_DEBUG SKY_EXPORT _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(skyborn PRIVATE _DEBUG SKY_EXPORT _CRT_SECURE_NO_WARNINGS)