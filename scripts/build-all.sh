#!/bin/sh

# NOTE: Untested!

echo "Calling add_source_files python script"
python scripts/add_source_files.py

if [ $? -eq 3 ]; then
    echo "Executing CMAKE to generate build system"
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang++.exe" -SD:/Skyborn -Bbuild -G "Unix Makefiles"
elif [ $? -ne 0 ]; then
    echo "Error:$?"
    exit $?
fi

echo "Building all..."

# NOTE: -j 26 signifies 26 threads. Your CPU may or may not support that many!
cmake --build build --config Debug --target all -j 26 --

echo "Done."
