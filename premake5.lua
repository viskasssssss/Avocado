workspace "Avocado"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    outputdir = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

includeDir = {}
includeDir["GLFW"] = "thirdparty/GLFW/include"
includeDir["spdlog"] = "thirdparty/spdlog/include"
includeDir["VulkanSDK"] =  "%{VULKAN_SDK}/Include"

libraryDir = {}
libraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
libraryDir["GLFW"] = "thirdparty/GLFW/lib"
libraryDir["spdlog"] = "thirdparty/spdlog/lib"

Library = {}
Library["Vulkan"] = "%{libraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{libraryDir.VulkanSDK}/VkLayer_utils.lib"

group "Avocado"
    include "Avocado"
group ""