#pragma once

#include "pch.h"
#include "vk_shaders.h"
#include "backend/renderer.h"

#include <array>
#include <cstring>
#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct GraphicsPipelineInBundle
	{
		vk::Device device;
		vk::PhysicalDevice physical_device;
		std::vector<avocado::renderer_shader_stage> shader_stages;
		std::vector<avocado::renderer_push_constant> push_constants;
		vk::Extent2D swapchain_extent;
		vk::Format swapchain_image_format;
	};

	struct GraphicsPipelineOutBundle
	{
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;
		vk::DescriptorSetLayout descriptor_set_layout;
		vk::DescriptorPool descriptor_pool;
		vk::DescriptorSet descriptor_set;
		vk::Buffer uniform_buffer;
		vk::DeviceMemory uniform_buffer_memory;
	};

	uint32_t find_memory_type(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memory_properties = physical_device.getMemoryProperties();
		for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
		{
			if ((type_filter & (1u << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		return 0;
	}

	vk::Buffer make_buffer(
		vk::Device device,
		vk::PhysicalDevice physical_device,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::DeviceMemory& memory
	)
	{
		vk::BufferCreateInfo buffer_info = {};
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = vk::SharingMode::eExclusive;

		try {
			vk::Buffer buffer = device.createBuffer(buffer_info);
			vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(buffer);
			uint32_t memory_type = find_memory_type(physical_device, requirements.memoryTypeBits, properties);

			vk::MemoryAllocateInfo alloc_info = {};
			alloc_info.allocationSize = requirements.size;
			alloc_info.memoryTypeIndex = memory_type;
			memory = device.allocateMemory(alloc_info);
			device.bindBufferMemory(buffer, memory, 0);
			return buffer;
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create buffer");
		}

		return vk::Buffer{};
	}

	vk::DescriptorSetLayout make_descriptor_set_layout(vk::Device device)
	{
		vk::DescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = vk::DescriptorType::eUniformBuffer;
		binding.descriptorCount = 1;
		binding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.bindingCount = 1;
		layout_info.pBindings = &binding;

		try {
			return device.createDescriptorSetLayout(layout_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create descriptor set layout");
		}

		return vk::DescriptorSetLayout{};
	}

	vk::DescriptorPool make_descriptor_pool(vk::Device device)
	{
		vk::DescriptorPoolSize pool_size = {};
		pool_size.type = vk::DescriptorType::eUniformBuffer;
		pool_size.descriptorCount = 1;

		vk::DescriptorPoolCreateInfo pool_info = {};
		pool_info.maxSets = 1;
		pool_info.poolSizeCount = 1;
		pool_info.pPoolSizes = &pool_size;

		try {
			return device.createDescriptorPool(pool_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create descriptor pool");
		}

		return vk::DescriptorPool{};
	}

	vk::DescriptorSet make_descriptor_set(vk::Device device, vk::DescriptorPool descriptor_pool, vk::DescriptorSetLayout descriptor_set_layout)
	{
		vk::DescriptorSetAllocateInfo alloc_info = {};
		alloc_info.descriptorPool = descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &descriptor_set_layout;

		try {
			std::vector<vk::DescriptorSet> descriptor_sets = device.allocateDescriptorSets(alloc_info);
			if (!descriptor_sets.empty())
				return descriptor_sets[0];
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to allocate descriptor set");
		}

		return vk::DescriptorSet{};
	}

	vk::PipelineLayout make_pipeline_layout(
		vk::Device device,
		uint32_t push_constant_count = 0,
		const vk::PushConstantRange* push_constants = nullptr,
		uint32_t descriptor_set_layout_count = 0,
		const vk::DescriptorSetLayout* descriptor_set_layouts = nullptr
	)
	{
		vk::PipelineLayoutCreateInfo layout_info = {};
		layout_info.flags = vk::PipelineLayoutCreateFlags();
		layout_info.setLayoutCount = descriptor_set_layout_count;
		layout_info.pSetLayouts = descriptor_set_layouts;
		layout_info.pushConstantRangeCount = push_constant_count;
		layout_info.pPushConstantRanges = push_constants;
		try {
			return device.createPipelineLayout(layout_info);
		} 
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create pipeline layout");
		}

		return vk::PipelineLayout{};
	}
	
	vk::ShaderStageFlagBits to_vulkan_shader_stage(avocado::shader_stage stage)
	{
		switch (stage)
		{
		case avocado::shader_stage::vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case avocado::shader_stage::fragment:
			return vk::ShaderStageFlagBits::eFragment;
		case avocado::shader_stage::geometry:
			return vk::ShaderStageFlagBits::eGeometry;
		default:
			return vk::ShaderStageFlagBits::eAll;
		}
	}

	vk::RenderPass make_renderpass(vk::Device device, vk::Format swapchain_image_format) 
	{
		vk::AttachmentDescription color_attachment = {};
		color_attachment.flags = vk::AttachmentDescriptionFlags();
		color_attachment.format = swapchain_image_format;
		color_attachment.samples = vk::SampleCountFlagBits::e1;
		color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		vk::RenderPassCreateInfo renderpass_info = {};
		renderpass_info.flags = vk::RenderPassCreateFlags();
		renderpass_info.attachmentCount = 1;
		renderpass_info.pAttachments = &color_attachment;
		renderpass_info.subpassCount = 1;
		renderpass_info.pSubpasses = &subpass;

		try {
			return device.createRenderPass(renderpass_info);
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create renderpass");
		}
		return vk::RenderPass{};
	}

	GraphicsPipelineOutBundle make_graphics_pipeline(GraphicsPipelineInBundle specification)
	{
		vk::GraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.flags = vk::PipelineCreateFlags();

		std::vector<vk::PipelineShaderStageCreateInfo> pipeline_shader_stages;
		std::vector<vk::ShaderModule> shader_modules;

		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.flags = vk::PipelineVertexInputStateCreateFlags();
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		pipeline_info.pVertexInputState = &vertex_input_info;

		vk::PipelineInputAssemblyStateCreateInfo input_assembly_info = {};
		input_assembly_info.flags = vk::PipelineInputAssemblyStateCreateFlags();
		input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
		pipeline_info.pInputAssemblyState = &input_assembly_info;

		for (const avocado::renderer_shader_stage& shader_stage : specification.shader_stages)
		{
			if (shader_stage.filepath.empty())
				continue;

			vk::ShaderModule shader = create_module(shader_stage.filepath, specification.device);
			vk::PipelineShaderStageCreateInfo shader_info = {};
			shader_info.flags = vk::PipelineShaderStageCreateFlags();
			shader_info.stage = to_vulkan_shader_stage(shader_stage.stage);
			shader_info.module = shader;
			shader_info.pName = "main";
			pipeline_shader_stages.push_back(shader_info);
			shader_modules.push_back(shader);
		}

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(specification.swapchain_extent.width);
		viewport.height = static_cast<float>(specification.swapchain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vk::Rect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent = specification.swapchain_extent;
		vk::PipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.flags = vk::PipelineViewportStateCreateFlags();
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;
		pipeline_info.pViewportState = &viewport_state;

		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		pipeline_info.pRasterizationState = &rasterizer;

		pipeline_info.stageCount = static_cast<uint32_t>(pipeline_shader_stages.size());
		pipeline_info.pStages = pipeline_shader_stages.data();

		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pipeline_info.pMultisampleState = &multisampling;

		vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		color_blend_attachment.blendEnable = VK_FALSE;
		vk::PipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.flags = vk::PipelineColorBlendStateCreateFlags();
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = vk::LogicOp::eCopy;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;
		pipeline_info.pColorBlendState = &color_blending;

		std::vector<vk::PushConstantRange> push_constant_ranges;
		for (const avocado::renderer_push_constant& push_constant : specification.push_constants)
		{
			if (push_constant.size == 0)
				continue;

			vk::PushConstantRange range = {};
			range.stageFlags = static_cast<vk::ShaderStageFlags>(push_constant.stage_flags);
			range.offset = push_constant.offset;
			range.size = push_constant.size;
			push_constant_ranges.push_back(range);
		}

		vk::DescriptorSetLayout descriptor_set_layout = make_descriptor_set_layout(specification.device);
		vk::DescriptorPool descriptor_pool = make_descriptor_pool(specification.device);
		vk::DescriptorSet descriptor_set = make_descriptor_set(specification.device, descriptor_pool, descriptor_set_layout);

		vk::Buffer uniform_buffer = vk::Buffer{};
		vk::DeviceMemory uniform_buffer_memory = vk::DeviceMemory{};
		const vk::DeviceSize uniform_buffer_size = sizeof(std::array<float, 4>);
		uniform_buffer = make_buffer(
			specification.device,
			specification.physical_device,
			uniform_buffer_size,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			uniform_buffer_memory
		);

		if (uniform_buffer && descriptor_set)
		{
			vk::DescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = uniform_buffer;
			buffer_info.offset = 0;
			buffer_info.range = uniform_buffer_size;

			vk::WriteDescriptorSet descriptor_write = {};
			descriptor_write.dstSet = descriptor_set;
			descriptor_write.dstBinding = 0;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptor_write.descriptorCount = 1;
			descriptor_write.pBufferInfo = &buffer_info;
			specification.device.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
		}

		vk::PipelineLayout layout = make_pipeline_layout(
			specification.device,
			static_cast<uint32_t>(push_constant_ranges.size()),
			push_constant_ranges.empty() ? nullptr : push_constant_ranges.data(),
			1,
			&descriptor_set_layout
		);
		pipeline_info.layout = layout;

		vk::RenderPass renderpass = make_renderpass(specification.device, specification.swapchain_image_format);
		pipeline_info.renderPass = renderpass;

		pipeline_info.basePipelineHandle = nullptr;

		AVO_TRACE("VK: Creating Graphics Pipeline...");
		vk::Pipeline graphics_pipeline;
		try {
			graphics_pipeline = (specification.device.createGraphicsPipeline(nullptr, pipeline_info)).value;
		}
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create Graphics Pipeline");
		}

		GraphicsPipelineOutBundle output = {};
		output.layout = layout;
		output.renderpass = renderpass;
		output.pipeline = graphics_pipeline;
		output.descriptor_set_layout = descriptor_set_layout;
		output.descriptor_pool = descriptor_pool;
		output.descriptor_set = descriptor_set;
		output.uniform_buffer = uniform_buffer;
		output.uniform_buffer_memory = uniform_buffer_memory;

		for (vk::ShaderModule shader_module : shader_modules)
			specification.device.destroyShaderModule(shader_module);
		return output;
	}
}