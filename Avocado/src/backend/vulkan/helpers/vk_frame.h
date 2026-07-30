#pragma once

#include <vulkan/vulkan.hpp>

namespace avo_vk 
{
	struct swapchain_frame
	{
		vk::Image image;
		vk::ImageView image_view;
		vk::Framebuffer framebuffer;
		vk::CommandBuffer command_buffer;
		vk::Semaphore image_available, render_finished;
		vk::Fence in_flight;
	};
}