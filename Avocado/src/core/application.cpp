#include "pch.h"
#include "application.h"

namespace avocado
{
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