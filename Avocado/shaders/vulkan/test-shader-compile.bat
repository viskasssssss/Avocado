@echo off
setlocal

if not defined VULKAN_SDK (
    echo VULKAN_SDK is not set. Please install the Vulkan SDK and set VULKAN_SDK.
    exit /b 1
)

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag fragment-test.hlsl -o fragment-test.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert vertex-test.hlsl -o vertex-test.spv

endlocal
PAUSE