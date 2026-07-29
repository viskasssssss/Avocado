#pragma once

#include "core/window.h"

#include <GLFW/glfw3.h>

namespace avocado
{
	class windows_window : public window
	{
	public:
		windows_window(const window_props& props);
		virtual ~windows_window();

		void on_update() override;

		inline unsigned int get_width() const override { return m_data.width; }
		inline unsigned int get_height() const override { return m_data.height; }

		inline void set_event_callback(const event_callback_fn& callback) override { m_data.event_callback = callback; }
		void set_vsync(bool enabled) override;
		bool is_vsync() const override;

		GLFWwindow* GetRaw()
		{
			return m_window.get();
		}
	private:
		virtual void init(const window_props& props);
		virtual void shutdown();
		
		std::shared_ptr<GLFWwindow> m_window;

		struct window_data
		{
			std::string title;
			unsigned int width, height;
			bool vsync;

			event_callback_fn event_callback;
		};

		window_data m_data;
	};
}