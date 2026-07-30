#pragma once

#include "pch.h"

#include "vk_frame.h"
#include "vk_queue_families.h"

#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct command_buffer_input_chunk {
		vk::Device device;
		vk::CommandPool command_pool;
		std::vector<swapchain_frame>& frames;
	};

	vk::CommandPool make_command_pool(
		vk::Device device, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) {

		queue_family_indices queue_family_indices = find_queue_families(physical_device, surface);

		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

		try {
			return device.createCommandPool(pool_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create Command Pool");

			return nullptr;
		}
	}

	vk::CommandBuffer make_command_buffers(command_buffer_input_chunk input_chunk) {

		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = input_chunk.command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;

		for (int i = 0; i < input_chunk.frames.size(); ++i) {
			try {
				input_chunk.frames[i].command_buffer = input_chunk.device.allocateCommandBuffers(alloc_info)[0];

				AVO_TRACE("VK: Allocated command buffer for frame {0}", i);
			}
			catch (vk::SystemError err) {
				AVO_TRACE("VK: Failed to allocate command buffer for frame {0}", i);
			}
		}

		try {
			vk::CommandBuffer command_buffer = input_chunk.device.allocateCommandBuffers(alloc_info)[0];

			AVO_TRACE("VK: Allocated main command buffer");

			return command_buffer;
		}
		catch (vk::SystemError err) {
			AVO_TRACE("VK: Failed to allocate main command buffer");

			return nullptr;
		}
	}
}