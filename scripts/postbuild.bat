@echo off
echo "Warning: This script is designed only for DEBUG builds"

if not exist "%cd%\bin\assets\shaders" mkdir "%cd%\bin\assets\shaders"

echo "Compiling shaders"

echo "assets/shaders/Object_vert.glsl --> bin/assets/shaders/Object_vert.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=vert assets/shaders/Object_vert.glsl -o bin/assets/shaders/Object_vert.spv

if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Object_frag.glsl --> bin/assets/shaders/Object_frag.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=frag assets/shaders/Object_frag.glsl -o bin/assets/shaders/Object_frag.spv

if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets"
echo xcopy "assets" "bin\assets" /h /i /c /k /e /r /y
xcopy "assets" "bin\assets" /h /i /c /k /e /r /y

echo "Done!"