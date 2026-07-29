#pragma once

#include "pch.h"
#include "defines.h"

namespace avocado
{
	struct window_props
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		window_props(const std::string& title = "avocado window",
			unsigned int width = 1280, unsigned int height = 720)
			: title(title), width(width), height(height) {}
	};

	class window
	{
	public:
		using event_callback_fn = std::function<void(avo_event&)>;

		virtual ~window() {}

		virtual void on_update() = 0;

		virtual unsigned int get_width() const = 0;
		virtual unsigned int get_height() const = 0;

		virtual void set_event_callback(const event_callback_fn& callback) = 0;
		virtual void set_vsync(bool enabled) = 0;
		virtual bool is_vsync() const = 0;

		static window* create(const window_props& props = window_props());
	};
}