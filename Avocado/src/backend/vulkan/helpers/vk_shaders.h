#pragma once

#include "pch.h"

#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace avo_vk
{
    std::string resolve_shader_path(const std::string& filename)
    {
        const std::filesystem::path input_path = filename;
        const std::filesystem::path cwd = std::filesystem::current_path();
        const std::vector<std::filesystem::path> candidates = {
            input_path,
            cwd / input_path,
            cwd / "Avocado" / input_path,
            cwd / ".." / ".." / ".." / "Avocado" / input_path,
            cwd / ".." / ".." / ".." / ".." / "Avocado" / input_path
        };

        for (const std::filesystem::path& candidate : candidates)
        {
            if (std::filesystem::exists(candidate))
                return candidate.string();
        }

        return filename;
    }

    std::vector<char> read_file(const std::string& filename) {
        const std::string resolved_path = resolve_shader_path(filename);
        std::ifstream file(resolved_path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            AVO_CRITICAL("VK: Failed to read file '{0}'", resolved_path.c_str());
            return {};
        }

        std::streamsize filesize = file.tellg();
        if (filesize < 0) {
            AVO_CRITICAL("VK: Failed to determine size for file '{0}'", resolved_path.c_str());
            return {};
        }

        std::vector<char> buffer(static_cast<size_t>(filesize));
        file.seekg(0, std::ios::beg);

        if (!file.read(buffer.data(), filesize)) {
            AVO_CRITICAL("VK: Failed to read data from file '{0}'", resolved_path.c_str());
            return {};
        }

        return buffer;
    }

    vk::ShaderModule create_module(std::string filename, vk::Device device)
    {
        std::vector<char> source_code = read_file(filename);
        vk::ShaderModuleCreateInfo module_info = {};
        module_info.flags = vk::ShaderModuleCreateFlags();
        module_info.codeSize = source_code.size();
        module_info.pCode = reinterpret_cast<const uint32_t*>(source_code.data());

        try {
            return device.createShaderModule(module_info);
        }
        catch (vk::SystemError err) {
            AVO_CRITICAL("VK: Failed to create shader module for {0}", filename.c_str());
        }

        return vk::ShaderModule{};
    }
}