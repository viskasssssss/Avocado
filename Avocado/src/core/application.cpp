#include "pch.h"
#include "application.h"

namespace avocado
{
	application::application()
	{
		m_window = scope<window>(window::create());
		m_window->set_event_callback(std::bind(&application::on_event, this, std::placeholders::_1));
	}

	void application::run()
	{
		while (m_running)
		{
			m_window->on_update();
		}
	}

	void application::on_event(avo_event& e)
	{
		event_dispatcher dispatcher(e);
		dispatcher.dispatch<window_close_event>(std::bind(&application::on_window_close, this, std::placeholders::_1));

		AVO_TRACE("{0}", e.to_string().c_str());
	}

	bool application::on_window_close(window_close_event& e)
	{
		m_running = false;
		return true;
	}
}