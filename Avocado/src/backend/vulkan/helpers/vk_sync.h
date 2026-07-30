#pragma once

#include "pch.h"

#include <vulkan/vulkan.hpp>

namespace avo_vk 
{
	vk::Semaphore make_semaphore(vk::Device device) {

		vk::SemaphoreCreateInfo semaphore_info = {};
		semaphore_info.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(semaphore_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create semaphore");
			return nullptr;
		}
	}

	vk::Fence make_fence(vk::Device device) {

		vk::FenceCreateInfo fence_info = {};
		fence_info.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {
			return device.createFence(fence_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create fence");
			return nullptr;
		}
	}
}