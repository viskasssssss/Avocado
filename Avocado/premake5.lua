project "Avocado"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin/int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files 
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "thirdparty",
        "%{includeDir.GLFW}",
        "%{includeDir.spdlog}",
        "%{includeDir.VulkanSDK}"
    }

    libdirs
    {
        "%{libraryDir.GLFW}",
        "%{libraryDir.spdlog}",
        "%{libraryDir.VulkanSDK}"
    }

    links
    {
        "glfw3",
        "opengl32",
        "spdlogd",
        "vulkan-1"
    }

    filter "system:windows"
        systemversion "latest"

        buildoptions { "/utf-8" }

        defines
        {
            "AVO_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "AVO_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "AVO_RELEASE"
        runtime "Release"
        optimize "on"