#!/bin/bash


mkdir -p bin/assets
mkdir -p bin/assets/shaders

echo "Compiling shaders..."

echo "assets/shaders/Object_vert.glsl --> bin/assets/shaders/Object_vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert assets/shaders/Object_vert.glsl -o bin/assets/shaders/Object_vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "assets/shaders/Object_frag.glsl --> bin/assets/shaders/Object_frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag assets/shaders/Object_frag.glsl -o bin/assets/shaders/Object_frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Copying assets..."
echo cp -R "assets" "bin"
cp -R "assets" "bin"

echo "Done."