#pragma once

#include "pch.h"
#include "vk_logging.h"
#include "vk_queue_families.h"
#include "vk_frame.h"

#include <set>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct swapchain_support_details
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct swapchain_bundle
	{
		vk::SwapchainKHR swapchain;
		std::vector<swapchain_frame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

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

		std::vector<vk::Image> images = logical_device.getSwapchainImagesKHR(bundle.swapchain);
		bundle.frames.resize(images.size());

		for (size_t i = 0; i < images.size(); ++i)
		{
			vk::ImageViewCreateInfo create_info = {};
			create_info.image = images[i];
			create_info.viewType = vk::ImageViewType::e2D;
			create_info.components.r = vk::ComponentSwizzle::eIdentity;
			create_info.components.g = vk::ComponentSwizzle::eIdentity;
			create_info.components.b = vk::ComponentSwizzle::eIdentity;
			create_info.components.a = vk::ComponentSwizzle::eIdentity;
			create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;
			create_info.format = format.format;

			bundle.frames[i].image = images[i];
			bundle.frames[i].image_view = logical_device.createImageView(create_info);
		}
		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
	}

}