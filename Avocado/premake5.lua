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
        "%{includeDir.spdlog}"
    }

    libdirs
    {
        "%{libraryDir.GLFW}",
        "%{libraryDir.spdlog}"
    }

    links
    {
        "glfw3",
        "opengl32",
        "spdlogd"
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