#pragma once

#include "backend/renderer.h"

#include "helpers/vk_frame.h"

#include <vulkan/vulkan.hpp>

namespace avocado
{
	class vulkan_renderer : public renderer
	{
	public:
		vulkan_renderer(const renderer_props& props);
		virtual ~vulkan_renderer();

		void on_update() override;
	private:
		void init(const renderer_props& props);
		void shutdown();

		void make_instance();
		void make_device();
		void make_pipeline();
		void finalize_setup();
		void record_draw_commands(vk::CommandBuffer command_buffer, uint32_t image_index);
		void render();

		struct renderer_data
		{
			ref<window> window;
		};

		renderer_data m_data;

		vk::Instance instance{ nullptr };
		vk::DebugUtilsMessengerEXT debug_messenger{ nullptr };
		vk::detail::DispatchLoaderDynamic dldi;
		vk::SurfaceKHR surface;

		vk::PhysicalDevice physical_device{ nullptr };
		vk::Device device{ nullptr };
		vk::Queue graphics_queue{ nullptr };
		vk::Queue present_queue{ nullptr };
		vk::SwapchainKHR swapchain{ nullptr };
		std::vector<avo_vk::swapchain_frame> swapchain_frames;
		vk::Format swapchain_format;
		vk::Extent2D swapchain_extent;

		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;

		vk::CommandPool command_pool;
		vk::CommandBuffer main_command_buffer;

		int max_frames_in_flight, frame_number;
	};
}