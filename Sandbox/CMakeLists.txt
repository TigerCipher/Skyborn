
set(SOURCE_FILES src/Game.cpp src/Main.cpp )

add_executable(sandbox ${SOURCE_FILES})
target_include_directories(sandbox PRIVATE ../engine/src src)
# add_compile_definitions(_DEBUG SKY_IMPORT _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(sandbox PUBLIC _DEBUG SKY_IMPORT _CRT_SECURE_NO_WARNINGS)
target_link_libraries(sandbox skyborn)