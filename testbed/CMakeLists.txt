
set(SOURCE_FILES src/Main.cpp src/TestManager.cpp src/Tests/BitsTest.cpp src/Tests/HeapArrayTest.cpp src/Tests/MathsTest.cpp src/Tests/VectorTest.cpp )

add_executable(testbed ${SOURCE_FILES})
target_include_directories(testbed PRIVATE ../engine/src src)
# add_compile_definitions(_DEBUG SKY_IMPORT _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(testbed PUBLIC _DEBUG SKY_IMPORT _CRT_SECURE_NO_WARNINGS)
target_link_libraries(testbed skyborn)