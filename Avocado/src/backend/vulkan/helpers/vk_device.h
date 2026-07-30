#pragma once

#include "pch.h"
#include "vk_logging.h"
#include "vk_queue_families.h"

#include <set>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	bool check_device_extension_support(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requested_extensions
	)
	{
		std::set<std::string> required_extensions(requested_extensions.begin(), requested_extensions.end());

		AVO_TRACE("VK: Device can support extensions:");
		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			AVO_TRACE("VK: \t{0}", (const char*)extension.extensionName);

			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	bool is_suitable(const vk::PhysicalDevice& device)
	{
		AVO_TRACE("VK: Checking if device is suitable");

		const std::vector<const char*> requested_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		AVO_TRACE("VK: Requesting device extensions:");
		for (const char* extension : requested_extensions) AVO_TRACE("VK: \t{0}", extension);

		if (bool extensions_supported = check_device_extension_support(device, requested_extensions))
		{
			AVO_TRACE("VK: Device can support the requested extensions");
			return true;
		}
		else
		{
			AVO_ASSERT(false);

			return false;
		}
	}

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance)
	{
		AVO_TRACE("VK: Choosing physical device...");

		std::vector<vk::PhysicalDevice> available_devices = instance.enumeratePhysicalDevices();

		AVO_TRACE("VK: There are {0} physical devices available on this system", available_devices.size());

		for (vk::PhysicalDevice device : available_devices)
		{
#ifdef AVO_DEBUG
			log_device_properties(device);
#endif
			if (is_suitable(device))
			{
				return device;
			}
		}

		return nullptr;
	}

	vk::Device create_logical_device(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface)
	{
		queue_family_indices indices = find_queue_families(physical_device, surface);
		std::vector<uint32_t> unique_indices;
		unique_indices.push_back(indices.graphics_family.value());
		if (indices.graphics_family.value() != indices.present_family.value())
			unique_indices.push_back(indices.present_family.value());
		float queue_priority = 1.0f;

		std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
		for (uint32_t queue_family_index : unique_indices)
		{
			queue_create_info.push_back(vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(), indices.graphics_family.value(),
					1, &queue_priority
				)
			);
		}

		std::vector<const char*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		vk::PhysicalDeviceFeatures device_features = vk::PhysicalDeviceFeatures();
		
		std::vector<const char*> enabled_layers;
		
#ifdef AVO_DEBUG
		enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		vk::DeviceCreateInfo device_info = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), 
			(uint32_t)queue_create_info.size(), queue_create_info.data(), 
			(uint32_t)enabled_layers.size(), enabled_layers.data(), 
			(uint32_t)device_extensions.size(), device_extensions.data(),
			&device_features
		);

		try 
		{
			vk::Device device = physical_device.createDevice(device_info);
			AVO_TRACE("VK: GPU has been successfully abstracted");
			return device;
		}
		catch (vk::SystemError err)
		{
			AVO_ASSERT(false);

			return nullptr;
		}
	}

	std::array<vk::Queue, 2> get_queues(vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface)
	{
		queue_family_indices indices = find_queue_families(physical_device, surface);

		return { {
				device.getQueue(indices.graphics_family.value(), 0),
				device.getQueue(indices.present_family.value(), 0)
			} };
	}

	
}