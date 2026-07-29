#pragma once

#include "defines.h"
#include "events/built_in_events.h"
#include "window.h"

namespace avocado
{
	class application
	{
	public:
		application();;
		
		void run();

		void on_event(avo_event& e);
	private:
		bool on_window_close(window_close_event& e);

		scope<window> m_window;
		bool m_running = true;
	};
}