#include "pch.h"
#include "application.h"

namespace avocado
{
	// TODO: temporary
	struct constants
	{
		std::array<float, 4> color;
	};

	application::application()
	{
		m_window = window::create();
		m_window->set_event_callback(std::bind(&application::on_event, this, std::placeholders::_1));

		m_renderer = renderer::create(renderer_props(m_window));

		
	}

	void application::run()
	{
		while (m_running)
		{
			m_window->on_update();
			constants c{ {0.0f, 1.0f, 0.0f, 1.0f} };

			avocado::renderer_push_constant pc{};
			pc.stage_flags = static_cast<uint32_t>(avocado::shader_stage::vertex);
			pc.offset = 0;
			pc.size = sizeof(constants);
			pc.data.resize(pc.size);
			std::memcpy(pc.data.data(), &c, sizeof(c));
			m_renderer->push_constants(pc);
			m_renderer->on_update();
		}
	}

	void application::on_event(avo_event& e)
	{
		event_dispatcher dispatcher(e);
		dispatcher.dispatch<window_close_event>(std::bind(&application::on_window_close, this, std::placeholders::_1));
	}

	bool application::on_window_close(window_close_event& e)
	{
		m_running = false;
		return true;
	}
}