# Avocado
Avocado is a modular video editing engine prototype for Windows, built around a plugin-driven architecture.

## Building

>Building Avocado is complex at this stage of development and will be changed in the future.

Requirements:
- Visual Studio 2022
- Vulkan SDK

### Building

Go to your environment variables and add a variable named "VULKAN_SDK" with the path to your Vulkan SDK installation (e.g., C:/VulkanSDK/[your version]).

To build *Avocado* you need to first build thirdparty requirements:
- GLFW
- spdlog

> Make sure you build them as dynamic libraries

Once you have build them, copy the results to the appropriate folders, for example: [your_path_to_avocado]/Avocado/GLFW/lib

Once you have completed the steps above, open the "build" folder, run the "win_generate_project.bat" script, and ensure there are no errors. After that, open the solution and build it.

You can find the result at the following path: bin/Release.[your system].x86_64/Avocado