#pragma once

#include "pch.h"

#include "vk_frame.h"

#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct framebuffer_input {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchain_extent;
	};

	void make_framebuffers(framebuffer_input input_chunk, std::vector<swapchain_frame>& frames) {

		for (int i = 0; i < frames.size(); ++i) {

			std::vector<vk::ImageView> attachments = {
				frames[i].image_view
			};

			vk::FramebufferCreateInfo framebuffer_info;
			framebuffer_info.flags = vk::FramebufferCreateFlags();
			framebuffer_info.renderPass = input_chunk.renderpass;
			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = input_chunk.swapchain_extent.width;
			framebuffer_info.height = input_chunk.swapchain_extent.height;
			framebuffer_info.layers = 1;

			try {
				frames[i].framebuffer = input_chunk.device.createFramebuffer(framebuffer_info);

				AVO_TRACE("VK: Created framebuffer for frame {0}", i);
			}
			catch (vk::SystemError err) {
				AVO_CRITICAL("VK: Failed to create framebuffer for frame {0}", i);
			}

		}
	}
}