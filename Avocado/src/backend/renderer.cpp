#include "pch.h"
#include "renderer.h"
#include "backend/vulkan/vulkan_renderer.h"

namespace avocado
{
	ref<renderer> renderer::create(const renderer_props& props)
	{
		switch (props.backend)
		{
		case renderer_backend_vulkan:
			return create_ref<vulkan_renderer>(props);
		default:
			AVO_ASSERT(false);
			return false;
		}
	}
}