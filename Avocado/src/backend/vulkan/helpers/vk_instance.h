#pragma once

#include "pch.h"

#include <vulkan/vulkan.hpp>

#ifdef AVO_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif

namespace avo_vk
{
	bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers)
	{
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		AVO_TRACE("VK: Device can support the following extensions:");
		for (vk::ExtensionProperties supportedExtension : supportedExtensions) { AVO_TRACE("VK: \t{0}", (const char*)supportedExtension.extensionName); }
		
		bool found;
		for (const char* extension : extensions)
		{
			found = false;
			for (vk::ExtensionProperties supportedExtension : supportedExtensions)
			{
				if (strcmp(extension, supportedExtension.extensionName) == 0) found = true;
			}
			if (!found) return false;
		}

		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		AVO_TRACE("VK: Device can support the following layers:");
		for (vk::LayerProperties supportedLayer : supportedLayers) { AVO_TRACE("VK: \t{0}", (const char*)supportedLayer.layerName); }

		for (const char* layer : layers)
		{
			found = false;
			for (vk::LayerProperties supportedLayer : supportedLayers)
			{
				if (strcmp(layer, supportedLayer.layerName) == 0) found = true;
			}
			if (!found) return false;
		}

		return true;
	}

	vk::Instance make_instance(const char* applicationName)
	{
		AVO_TRACE("VK: Creating an instance...");

		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);

		AVO_TRACE("VK: System can support vulkan version {0}.{1}.{2}.{3}", 
			VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), 
			VK_API_VERSION_PATCH(version), VK_API_VERSION_VARIANT(version));

		version &= ~(0xFFFU);

		version = VK_MAKE_API_VERSION(0, 1, 0, 0);

		vk::ApplicationInfo app_info = vk::ApplicationInfo(
			applicationName,
			version,
			"Avocado",
			version,
			version
		);
		 
#ifdef AVO_PLATFORM_WINDOWS
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
#endif

#ifdef AVO_DEBUG
		extensions.push_back("VK_EXT_debug_utils");
#endif

		AVO_TRACE("VK: Extensions to be required:");
		for (const char* extension_name : extensions) { AVO_TRACE("VK: \t{0}", extension_name); }

		std::vector<const char*> layers;

#ifdef AVO_DEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		if (!supported(extensions, layers)) AVO_ASSERT(false);

		vk::InstanceCreateInfo create_info = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&app_info,
			static_cast<uint32_t>(layers.size()), layers.data(),
			static_cast<uint32_t> (extensions.size()), extensions.data()
		);

		try {
			return vk::createInstance(create_info, nullptr);
		}
		catch (vk::SystemError err)
		{
			AVO_ASSERT(false, "Vulkan instance creation failed");

			return nullptr;
		}
	}
}