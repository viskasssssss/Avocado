#include "pch.h"
#include "vulkan_renderer.h"

#include "helpers/vk_instance.h"
#include "helpers/vk_logging.h"
#include "helpers/vk_device.h"

namespace avocado
{
	ref<renderer> renderer::create(const renderer_props& props)
	{
		return create_ref<vulkan_renderer>(props);
	}

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
		// TODO: implement Vulkan rendering update logic
	}

	void vulkan_renderer::init(const renderer_props& props)
	{
		m_data.window = props.window;

		make_instance();
		AVO_INFO("VK: Created Vulkan instance");
		make_device();
		AVO_INFO("VK: Created device");
	}

	void vulkan_renderer::shutdown()
	{
		device.destroy();
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
	}

	void vulkan_renderer::make_device()
	{
		physical_device = avo_vk::choose_physical_device(instance);
		device = avo_vk::create_logical_device(physical_device);
		graphics_queue = avo_vk::get_queue(physical_device, device);
	}
}