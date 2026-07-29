#pragma once

#include "defines.h"
#include "events/built_in_events.h"
#include "window.h"

#include "backend/renderer.h"

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

		ref<renderer> m_renderer;
		ref<window> m_window;
		bool m_running = true;
	};
}