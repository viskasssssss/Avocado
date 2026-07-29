#pragma once

#include "core/window.h"
#include "core/defines.h"

namespace avocado
{
	enum renderer_backend
	{
		renderer_backend_vulkan
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
		virtual ~renderer() {}

		virtual void on_update() = 0;

		static ref<renderer> create(const renderer_props& props);
	};
}