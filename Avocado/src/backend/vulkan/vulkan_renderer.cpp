#include "pch.h"
#include "vulkan_renderer.h"

#include "helpers/vk_instance.h"
#include "helpers/vk_logging.h"
#include "helpers/vk_device.h"

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
		swapchain_images = bundle.images;
		swapchain_format = bundle.format;
		swapchain_extent = bundle.extent;
	}
}