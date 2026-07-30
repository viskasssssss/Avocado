#pragma once

#include "pch.h"

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
}