#pragma once

#include "pch.h"
#include "vk_shaders.h"

#include <vulkan/vulkan.hpp>

namespace avo_vk
{
	struct GraphicsPipelineInBundle
	{
		vk::Device device;
		std::string vertex_filepath;
		std::string fragment_filepath;
		vk::Extent2D swapchain_extent;
		vk::Format swapchain_image_format;
	};

	struct GraphicsPipelineOutBundle
	{
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;
	};

	vk::PipelineLayout make_pipeline_layout(vk::Device device)
	{
		vk::PipelineLayoutCreateInfo layout_info;
		layout_info.flags = vk::PipelineLayoutCreateFlags();
		layout_info.setLayoutCount = 0;
		layout_info.pushConstantRangeCount = 0;
		try {
			return device.createPipelineLayout(layout_info);
		} 
		catch (vk::SystemError err) {
			AVO_CRITICAL("VK: Failed to create pipeline layout");
		}

		return vk::PipelineLayout{};
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

		std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.flags = vk::PipelineVertexInputStateCreateFlags();
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		pipeline_info.pVertexInputState = &vertex_input_info;

		vk::PipelineInputAssemblyStateCreateInfo input_assembly_info = {};
		input_assembly_info.flags = vk::PipelineInputAssemblyStateCreateFlags();
		input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
		pipeline_info.pInputAssemblyState = &input_assembly_info;

		vk::ShaderModule vertex_shader = create_module(specification.vertex_filepath, specification.device);
		vk::PipelineShaderStageCreateInfo vertex_shader_info = {};
		vertex_shader_info.flags = vk::PipelineShaderStageCreateFlags();
		vertex_shader_info.stage = vk::ShaderStageFlagBits::eVertex;
		vertex_shader_info.module = vertex_shader;
		vertex_shader_info.pName = "main";
		shader_stages.push_back(vertex_shader_info);

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

		vk::ShaderModule fragment_shader = create_module(specification.fragment_filepath, specification.device);
		vk::PipelineShaderStageCreateInfo fragment_shader_info = {};
		fragment_shader_info.flags = vk::PipelineShaderStageCreateFlags();
		fragment_shader_info.stage = vk::ShaderStageFlagBits::eFragment;
		fragment_shader_info.module = fragment_shader;
		fragment_shader_info.pName = "main";
		shader_stages.push_back(fragment_shader_info);
		pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
		pipeline_info.pStages = shader_stages.data();

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

		vk::PipelineLayout layout = make_pipeline_layout(specification.device);
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

		specification.device.destroyShaderModule(vertex_shader);
		specification.device.destroyShaderModule(fragment_shader);
		return output;
	}
}