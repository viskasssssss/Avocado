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

	struct swapchain_support_details
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct swapchain_bundle
	{
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;
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

	queue_family_indices find_queue_families(vk::PhysicalDevice device, vk::SurfaceKHR surface)
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

				AVO_TRACE("VK: Queue family {0} is suitable for graphics", i);
			}

			if (device.getSurfaceSupportKHR(i, surface))
			{
				indices.present_family = i;

				AVO_TRACE("VK: Queue family {0} is suitable for presenting", i);
			}

			if (indices.is_complete()) break;

			i++;
		}
		
		return indices;
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

	swapchain_support_details query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		swapchain_support_details support;

		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

		AVO_TRACE("VK: Swapchain can support the following surface capabiliities:");
		AVO_TRACE("VK: \t Minimum image count: {0}", support.capabilities.minImageCount);
		AVO_TRACE("VK: \t Maximum image count: {0}", support.capabilities.maxImageCount);

		AVO_TRACE("VK: \t\t Current extent: ");
		AVO_TRACE("VK: \t\t Width: {0}", support.capabilities.currentExtent.width);
		AVO_TRACE("VK: \t\t Height: {0}", support.capabilities.currentExtent.height);
						 
		AVO_TRACE("VK: \t\t Minimum supported extent: ");
		AVO_TRACE("VK: \t\t Width: {0}", support.capabilities.minImageExtent.width);
		AVO_TRACE("VK: \t\t Height: {0}", support.capabilities.minImageExtent.height);
						 
		AVO_TRACE("VK: \t\t Maximum supported extent: ");
		AVO_TRACE("VK: \t\t Width: {0}", support.capabilities.maxImageExtent.width);
		AVO_TRACE("VK: \t\t Height: {0}", support.capabilities.maxImageExtent.height);

		AVO_TRACE("VK: \t Maximum image array layers: {0}", support.capabilities.maxImageArrayLayers);

		AVO_TRACE("VK: \t Supported transforms: ");
		std::vector<std::string> stringList = log_transform_bits(support.capabilities.supportedTransforms);
		for (std::string line : stringList) AVO_TRACE("VK: \t\t {0}", line.c_str());

		AVO_TRACE("VK: \t Supported transforms: ");
		stringList = log_transform_bits(support.capabilities.currentTransform);
		for (std::string line : stringList) AVO_TRACE("VK: \t\t {0}", line.c_str());

		AVO_TRACE("VK: \t Supported alpha operations: ");
		stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
		for (std::string line : stringList) AVO_TRACE("VK: \t\t {0}", line.c_str());

		AVO_TRACE("VK: \t Supported image usage: ");
		stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
		for (std::string line : stringList) AVO_TRACE("VK: \t\t {0}", line.c_str());

		support.formats = device.getSurfaceFormatsKHR(surface);

		for (vk::SurfaceFormatKHR supported_format : support.formats)
		{
			AVO_TRACE("VK: Supported pixel format: {0}", vk::to_string(supported_format.format));
			AVO_TRACE("VK: Supported color space: {0}", vk::to_string(supported_format.colorSpace));
		}

		support.present_modes = device.getSurfacePresentModesKHR(surface);

		for (vk::PresentModeKHR present_mode : support.present_modes)
		{
			AVO_TRACE("VK: \t{0}", log_present_mode(present_mode));
		}

		return support;
	}

	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats)
	{
		for (vk::SurfaceFormatKHR format : formats)
		{
			if (format.format == vk::Format::eB8G8R8A8Unorm
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) return format;
		}

		return formats[0];
	}

	vk::PresentModeKHR choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> present_modes)
	{
		for (vk::PresentModeKHR present_mode : present_modes)
		{
			if (present_mode == vk::PresentModeKHR::eMailbox) return present_mode;
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;
		else
		{
			vk::Extent2D extent = { width, height };

			extent.width = std::min(
				capabilities.maxImageExtent.width,
				std::max(capabilities.minImageExtent.width, width)
			);

			extent.height = std::min(
				capabilities.maxImageExtent.height,
				std::max(capabilities.minImageExtent.width, height)
			);

			return extent;
		}
	}

	swapchain_bundle create_swapchain(vk::Device logical_device, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, int width, int height)
	{
		swapchain_support_details support = query_swapchain_support(physical_device, surface);

		vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);

		vk::PresentModeKHR present_mode = choose_swapchain_present_mode(support.present_modes);

		vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

		uint32_t image_count = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);

		vk::SwapchainCreateInfoKHR create_info = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, image_count,
			format.format, format.colorSpace, extent,
			1, vk::ImageUsageFlagBits::eColorAttachment
		);

		queue_family_indices indices = find_queue_families(physical_device, surface);
		uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family.value() != indices.present_family.value())
		{
			create_info.imageSharingMode = vk::SharingMode::eConcurrent;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else create_info.imageSharingMode = vk::SharingMode::eExclusive;

		create_info.preTransform = support.capabilities.currentTransform;
		create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = vk::SwapchainKHR(nullptr);

		swapchain_bundle bundle{};
		try 
		{
			bundle.swapchain = logical_device.createSwapchainKHR(create_info);
		}
		catch (vk::SystemError err)
		{
			AVO_ASSERT(false);
		}

		bundle.images = logical_device.getSwapchainImagesKHR(bundle.swapchain);
		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
	}

}