workspace "Avocado"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    outputdir = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"

includeDir = {}
includeDir["GLFW"] = "thirdparty/GLFW/include"
includeDir["spdlog"] = "thirdparty/spdlog/include"

libraryDir = {}
libraryDir["GLFW"] = "thirdparty/GLFW/lib"
libraryDir["spdlog"] = "thirdparty/spdlog/lib"

group "Avocado"
    include "Avocado"
group ""