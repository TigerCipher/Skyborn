@echo off

echo "Calling add_source_files python script"
call python scripts/add_source_files.py

if %ERRORLEVEL% == 3 (
    echo "Executing CMAKE to generate build system"
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang++.exe" -SD:/Skyborn -Bbuild -G "Unix Makefiles"
) else if %ERRORLEVEL% NEQ 0 ( echo Error:%ERRORLEVEL% && exit )

echo "Building all..."

REM NOTE: -j 26 signifies 26 threads. Your CPU may or may not support that many!
cmake --build build --config Debug --target all -j 26 --

echo "Done."