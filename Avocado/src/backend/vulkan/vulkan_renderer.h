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
		void create_default_pipeline_specification();
		void record_draw_commands(vk::CommandBuffer command_buffer, uint32_t image_index);
		void render();

		void render_start(const std::array<float, 4>& clear_color) override;
		void render_end() override;
		void bind_pipeline(const renderer_pipeline_specification& pipeline_spec) override;
		void push_constants(const renderer_push_constant& push_constant) override;
		void draw(uint32_t vertex_count, uint32_t instance_count = 1) override;

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
		vk::DescriptorSetLayout descriptor_set_layout;
		vk::DescriptorPool descriptor_pool;
		vk::DescriptorSet descriptor_set;
		vk::Buffer uniform_buffer;
		vk::DeviceMemory uniform_buffer_memory;

		vk::CommandPool command_pool;
		vk::CommandBuffer main_command_buffer;
		vk::CommandBuffer active_command_buffer;
		renderer_push_constant pending_push_constant;
		bool has_pending_push_constant = false;

		uint32_t current_image_index = 0;
		bool render_pass_begun = false;
		renderer_pipeline_specification default_pipeline_spec;
		renderer_pipeline_specification active_pipeline_spec;

		int max_frames_in_flight = 0;
		int frame_number = 0;
	};
}