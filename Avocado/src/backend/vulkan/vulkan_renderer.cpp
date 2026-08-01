#include "pch.h"
#include "vulkan_renderer.h"

#include "helpers/vk_instance.h"
#include "helpers/vk_logging.h"
#include "helpers/vk_device.h"
#include "helpers/vk_swapchain.h"
#include "helpers/vk_pipeline.h"
#include "helpers/vk_framebuffer.h"
#include "helpers/vk_commands.h"
#include "helpers/vk_sync.h"

#include "core/math.h"

#ifdef AVO_PLATFORM_WINDOWS
#include "platform/windows/windows_window.h"
#endif

namespace avocado
{
	vulkan_renderer::vulkan_renderer(const renderer_props& props)
	{
		init(props);
	}

	vulkan_renderer::~vulkan_renderer()
	{
		shutdown();
	}

	void vulkan_renderer::on_update()
	{
		render();
	}

	void vulkan_renderer::init(const renderer_props& props)
	{
		m_data.window = props.window;
		create_default_pipeline_specification();

		make_instance();
		AVO_INFO("VK: Created Vulkan instance");
		make_device();
		AVO_INFO("VK: Created device");
		make_pipeline();
		AVO_INFO("VK: Created pipeline");
		finalize_setup();
	}

	void vulkan_renderer::shutdown()
	{
		device.waitIdle();

		device.destroyCommandPool(command_pool);

		device.destroyPipeline(pipeline);
		device.destroyPipelineLayout(layout);
		device.destroyDescriptorSetLayout(descriptor_set_layout);
		device.destroyDescriptorPool(descriptor_pool);
		if (uniform_buffer) device.destroyBuffer(uniform_buffer);
		if (uniform_buffer_memory) device.freeMemory(uniform_buffer_memory);
		device.destroyRenderPass(renderpass);

		for (avo_vk::swapchain_frame frame : swapchain_frames)
		{
			device.destroyImageView(frame.image_view);
			device.destroyFramebuffer(frame.framebuffer);
			device.destroyFence(frame.in_flight);
			device.destroySemaphore(frame.image_available);
			device.destroySemaphore(frame.render_finished);
		}

		device.destroySwapchainKHR(swapchain);
		device.destroy();

		instance.destroySurfaceKHR(surface);
#ifdef AVO_DEBUG
		instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dldi);
#endif
		instance.destroy();
	}

	void vulkan_renderer::make_instance()
	{
		instance = avo_vk::make_instance("id"); // TODO: Actual ID
		dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

#ifdef AVO_DEBUG
		debug_messenger = avo_vk::make_debug_messenger(instance, dldi);
#endif
		
#ifdef AVO_PLATFORM_WINDOWS
		VkSurfaceKHR c_style_surface = VK_NULL_HANDLE;
		if (auto* raw_window = dynamic_cast<windows_window*>(m_data.window.get()))
		{
			if (glfwCreateWindowSurface(instance, raw_window->GetRaw(), nullptr, &c_style_surface) != VK_SUCCESS)
				AVO_ASSERT(false);
		}
		else AVO_ASSERT(false);

		surface = c_style_surface;
		AVO_INFO("VK: Successfully abstracted the GLFW surface for Vulkan");
#endif
	}

	void vulkan_renderer::make_device()
	{
		physical_device = avo_vk::choose_physical_device(instance);
		device = avo_vk::create_logical_device(physical_device, surface);
		std::array<vk::Queue,2> queues = avo_vk::get_queues(physical_device, device, surface);
		graphics_queue = queues[0];
		present_queue = queues[1];
		avo_vk::swapchain_bundle bundle = avo_vk::create_swapchain(
			device, physical_device, surface, m_data.window->get_width(), m_data.window->get_height()
		);
		swapchain = bundle.swapchain;
		swapchain_frames = bundle.frames;
		swapchain_format = bundle.format;
		swapchain_extent = bundle.extent;
		max_frames_in_flight = static_cast<int>(swapchain_frames.size());
		frame_number = 0;
	}

	void vulkan_renderer::make_pipeline()
	{
		avo_vk::GraphicsPipelineInBundle specification = {};
		specification.device = device;
		specification.physical_device = physical_device;
		specification.shader_stages = default_pipeline_spec.shader_stages;
		specification.push_constants = default_pipeline_spec.push_constants;
		specification.swapchain_extent = swapchain_extent;
		specification.swapchain_image_format = swapchain_format;

		avo_vk::GraphicsPipelineOutBundle output = avo_vk::make_graphics_pipeline(specification);
		layout = output.layout;
		renderpass = output.renderpass;
		pipeline = output.pipeline;
		descriptor_set_layout = output.descriptor_set_layout;
		descriptor_pool = output.descriptor_pool;
		descriptor_set = output.descriptor_set;
		uniform_buffer = output.uniform_buffer;
		uniform_buffer_memory = output.uniform_buffer_memory;
	}

	void vulkan_renderer::finalize_setup()
	{
		avo_vk::framebuffer_input framebuffer_input;
		framebuffer_input.device = device;
		framebuffer_input.renderpass = renderpass;
		framebuffer_input.swapchain_extent = swapchain_extent;
		avo_vk::make_framebuffers(framebuffer_input, swapchain_frames);

		command_pool = avo_vk::make_command_pool(device, physical_device, surface);

		avo_vk::command_buffer_input_chunk command_buffer_input = { device, command_pool, swapchain_frames };
		main_command_buffer = avo_vk::make_command_buffers(command_buffer_input);

		for (avo_vk::swapchain_frame& frame : swapchain_frames)
		{
			frame.in_flight = avo_vk::make_fence(device);
			frame.image_available = avo_vk::make_semaphore(device);
			frame.render_finished = avo_vk::make_semaphore(device);
		}
	}

	void vulkan_renderer::create_default_pipeline_specification()
	{
		// just a test for now
		default_pipeline_spec.shader_stages = {
			{"shaders/vulkan/vertex-test.spv", shader_stage::vertex},
			{"shaders/vulkan/fragment-test.spv", shader_stage::fragment}
		};

		renderer_push_constant push_constant = {};
		push_constant.stage_flags = static_cast<uint32_t>(shader_stage::vertex);
		push_constant.offset = 0;
		push_constant.size = sizeof(float) * 4;
		push_constant.data.resize(push_constant.size);
		default_pipeline_spec.push_constants.push_back(push_constant);
		default_pipeline_spec.clear_color = {0.25f, 0.25f, 0.3f, 1.0f};
		active_pipeline_spec = default_pipeline_spec;
	}

	void vulkan_renderer::render_start(const std::array<float, 4>& clear_color)
	{
		if (render_pass_begun)
			return;

		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = renderpass;
		render_pass_info.framebuffer = swapchain_frames[current_image_index].framebuffer;
		render_pass_info.renderArea.offset.x = 0;
		render_pass_info.renderArea.offset.y = 0;
		render_pass_info.renderArea.extent = swapchain_extent;

		vk::ClearValue clear_color_value = { std::array<float, 4>{clear_color[0], clear_color[1], clear_color[2], clear_color[3]} };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color_value;

		active_command_buffer.beginRenderPass(&render_pass_info, vk::SubpassContents::eInline);
		active_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		if (descriptor_set)
			active_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, 1, &descriptor_set, 0, nullptr);
		render_pass_begun = true;
	}

	void vulkan_renderer::render_end()
	{
		if (!render_pass_begun)
			return;

		active_command_buffer.endRenderPass();
		render_pass_begun = false;
	}

	void vulkan_renderer::bind_pipeline(const renderer_pipeline_specification& pipeline_spec)
	{
		active_pipeline_spec = pipeline_spec;
		if (active_command_buffer && render_pass_begun)
			active_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	}

	void vulkan_renderer::push_constants(const renderer_push_constant& push_constant)
	{
		if (!layout)
			return;

		const uint32_t size = push_constant.size ? push_constant.size : static_cast<uint32_t>(push_constant.data.size());
		if (size == 0 || push_constant.data.empty())
			return;

		if (active_command_buffer && render_pass_begun)
		{
			active_command_buffer.pushConstants(
				layout,
				static_cast<vk::ShaderStageFlags>(push_constant.stage_flags),
				push_constant.offset,
				size,
				push_constant.data.data()
			);
			return;
		}

		pending_push_constant = push_constant;
		has_pending_push_constant = true;
	}

	void vulkan_renderer::draw(uint32_t vertex_count, uint32_t instance_count)
	{
		if (!render_pass_begun)
			return;

		active_command_buffer.draw(vertex_count, instance_count, 0, 0);
	}

	void vulkan_renderer::record_draw_commands(vk::CommandBuffer command_buffer, uint32_t image_index)
	{
		current_image_index = image_index;
		active_command_buffer = command_buffer;
		render_pass_begun = false;

		vk::CommandBufferBeginInfo begin_info = {};
		try {
			command_buffer.begin(begin_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("Failed to begin recording command buffer");
		}

		render_start(active_pipeline_spec.clear_color);
		bind_pipeline(active_pipeline_spec);

		renderer_push_constant push_constant_to_use = pending_push_constant;
		if (push_constant_to_use.data.empty() || push_constant_to_use.size == 0)
		{
			if (!active_pipeline_spec.push_constants.empty())
				push_constant_to_use = active_pipeline_spec.push_constants.front();
		}

		if (!push_constant_to_use.data.empty() && push_constant_to_use.size != 0)
			push_constants(push_constant_to_use);

		draw(3, 1);
		render_end();

		try {
			command_buffer.end();
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("Failed to finish recording command buffer");
		}
	}

	void vulkan_renderer::render()
	{
		device.waitForFences(1, &swapchain_frames[frame_number].in_flight, VK_TRUE, UINT64_MAX);
		device.resetFences(1, &swapchain_frames[frame_number].in_flight);

		uint32_t image_index{ 
			device.acquireNextImageKHR(
				swapchain, UINT64_MAX, swapchain_frames[frame_number].image_available, nullptr
			).value
		};

		vk::CommandBuffer command_buffer = swapchain_frames[frame_number].command_buffer;

		command_buffer.reset();

		record_draw_commands(command_buffer, image_index);

		vk::SubmitInfo submit_info = {};
		vk::Semaphore wait_semaphores[] = { swapchain_frames[frame_number].image_available };
		vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		vk::Semaphore signal_semaphores[] = { swapchain_frames[frame_number].render_finished };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		try {
			graphics_queue.submit(submit_info, swapchain_frames[frame_number].in_flight);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("Failed to submit draw command buffer");
		}

		vk::PresentInfoKHR present_info = {};
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;
		vk::SwapchainKHR swapchains[] = { swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapchains;
		present_info.pImageIndices = &image_index;

		present_queue.presentKHR(present_info);

		frame_number = (frame_number + 1) % max_frames_in_flight;
	}
}