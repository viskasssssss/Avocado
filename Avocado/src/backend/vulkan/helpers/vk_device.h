#pragma once

#include <pch.h>
#include <set>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct queue_family_indices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		bool is_complete()
		{
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	void log_device_properties(const vk::PhysicalDevice& device)
	{
		vk::PhysicalDeviceProperties properties = device.getProperties();

		AVO_TRACE("VK: Device name: {0}", (const char*)properties.deviceName);
		
		AVO_TRACE("VK: Device type:");

		switch (properties.deviceType)
		{
		case (vk::PhysicalDeviceType::eCpu):
			AVO_TRACE("VK: \tCPU");
			break;
		case (vk::PhysicalDeviceType::eDiscreteGpu):
			AVO_TRACE("VK: \tDiscrete GPU");
			break;
		case (vk::PhysicalDeviceType::eIntegratedGpu):
			AVO_TRACE("VK: \tIntegrated GPU");
			break;
		case (vk::PhysicalDeviceType::eVirtualGpu):
			AVO_TRACE("VK: \tVirtual GPU");
			break;
		default:
			AVO_TRACE("VK: \tOther");
		}
	}

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

	queue_family_indices find_queue_families(vk::PhysicalDevice device)
	{
		queue_family_indices indices;

		std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties();

		AVO_TRACE("VK: System can support {0} queue families", queue_families.size());

		int i = 0;
		for (vk::QueueFamilyProperties queue_family : queue_families)
		{
			if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphics_family = i;
				indices.present_family = i;

				AVO_TRACE("VK: Queue family {0} is suitable for graphics and presenting", i);
			}

			if (indices.is_complete()) break;

			i++;
		}
		
		return indices;
	}

	vk::Device create_logical_device(vk::PhysicalDevice physical_device)
	{
		queue_family_indices indices = find_queue_families(physical_device);
		float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info = vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(), indices.graphics_family.value(),
			1, &queue_priority
		);


		vk::PhysicalDeviceFeatures device_features = vk::PhysicalDeviceFeatures();
		
		std::vector<const char*> enabled_layers;
		
#ifdef AVO_DEBUG
		enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		vk::DeviceCreateInfo device_info = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), 1, 
			&queue_create_info, (uint32_t)enabled_layers.size(), 
			enabled_layers.data(), 0, 
			nullptr, &device_features
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

	vk::Queue get_queue(vk::PhysicalDevice physical_device, vk::Device device)
	{
		queue_family_indices indices = find_queue_families(physical_device);

		return device.getQueue(indices.graphics_family.value(), 0);
	}
}