#pragma once

#include "core/window.h"
#include "core/defines.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace avocado
{
	enum renderer_backend
	{
		renderer_backend_vulkan
	};

	enum class shader_stage : uint32_t
	{
		none = 0,
		vertex = 1 << 0,
		fragment = 1 << 1,
		geometry = 1 << 2
	};

	struct renderer_shader_stage
	{
		std::string filepath;
		shader_stage stage;
	};

	struct renderer_push_constant
	{
		uint32_t stage_flags = 0;
		uint32_t offset = 0;
		uint32_t size = 0;
		std::vector<uint8_t> data;
	};

	struct renderer_pipeline_specification
	{
		std::vector<renderer_shader_stage> shader_stages;
		std::vector<renderer_push_constant> push_constants;
		std::array<float, 4> clear_color{0.5f, 0.5f, 0.5f, 1.0f};
	};

	struct renderer_props
	{
		ref<window> window;
		renderer_backend backend;

		renderer_props(ref<avocado::window> window, const renderer_backend& backend = renderer_backend_vulkan) : window(window), backend(backend) {}
	};

	class renderer
	{
	public:
		virtual ~renderer() = default;

		virtual void on_update() = 0;
		virtual void render_start(const std::array<float, 4>& clear_color = std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f}) = 0;
		virtual void render_end() = 0;
		virtual void bind_pipeline(const renderer_pipeline_specification& pipeline_spec) = 0;
		virtual void push_constants(const renderer_push_constant& push_constant) = 0;
		virtual void draw(uint32_t vertex_count, uint32_t instance_count = 1) = 0;

		static ref<renderer> create(const renderer_props& props);
	};
}