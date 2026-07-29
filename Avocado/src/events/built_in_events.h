#pragma once

#include <sstream>

#include "core/key_codes.h"
#include "core/mouse_codes.h"
#include "event.h"

namespace avocado
{
	class window_resize_event : public avo_event
	{
	public:
		window_resize_event(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {}

		unsigned int get_width() const { return m_width; }
		unsigned int get_height() const { return m_height; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "window_resize_event: " << m_width << ", " << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(window_resize)
		EVENT_CLASS_CATEGORY(event_category_application)

	private:
		unsigned int m_width, m_height;
	};

	class window_close_event : public avo_event
	{
	public:
		window_close_event() = default;

		EVENT_CLASS_TYPE(window_close)
		EVENT_CLASS_CATEGORY(event_category_application)
	};

	class app_tick_event : public avo_event
	{
	public:
		app_tick_event() = default;

		EVENT_CLASS_TYPE(app_tick)
		EVENT_CLASS_CATEGORY(event_category_application)
	};

	class app_update_event : public avo_event
	{
	public:
		app_update_event() = default;

		EVENT_CLASS_TYPE(app_update)
		EVENT_CLASS_CATEGORY(event_category_application)
	};

	class app_render_event : public avo_event
	{
	public:
		app_render_event() = default;

		EVENT_CLASS_TYPE(app_render)
		EVENT_CLASS_CATEGORY(event_category_application)
	};

	class key_event : public avo_event
	{
	public:
		key_code get_key_code() const { return m_key_code; }

		EVENT_CLASS_CATEGORY(event_category_keyboard | event_category_input)

	protected:
		key_event(const key_code keycode)
			: m_key_code(keycode) {}

		key_code m_key_code;
	};

	class key_pressed_event : public key_event
	{
	public:
		key_pressed_event(const key_code keycode, bool is_repeat = false)
			: key_event(keycode), m_is_repeat(is_repeat) {}

		bool is_repeat() const { return m_is_repeat; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_pressed_event: " << m_key_code
				<< " (repeat = " << m_is_repeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_pressed)

	private:
		bool m_is_repeat;
	};

	class key_released_event : public key_event
	{
	public:
		key_released_event(const key_code keycode)
			: key_event(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_released_event: " << m_key_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_released)
	};

	class key_typed_event : public key_event
	{
	public:
		key_typed_event(const key_code keycode)
			: key_event(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_typed_event: " << m_key_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_typed)
	};

	class mouse_moved_event : public avo_event
	{
	public:
		mouse_moved_event(float x, float y)
			: m_mouse_x(x), m_mouse_y(y) {}

		float get_x() const { return m_mouse_x; }
		float get_y() const { return m_mouse_y; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_moved_event: " << m_mouse_x << ", " << m_mouse_y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_moved)
			EVENT_CLASS_CATEGORY(event_category_mouse | event_category_input)

	private:
		float m_mouse_x, m_mouse_y;
	};

	class mouse_scrolled_event : public avo_event
	{
	public:
		mouse_scrolled_event(float x_offset, float y_offset)
			: m_x_offset(x_offset), m_y_offset(y_offset) {}

		float get_x_offset() const { return m_x_offset; }
		float get_y_offset() const { return m_y_offset; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_scrolled_event: "
				<< get_x_offset() << ", " << get_y_offset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_scrolled)
			EVENT_CLASS_CATEGORY(event_category_mouse | event_category_input)

	private:
		float m_x_offset, m_y_offset;
	};

	class mouse_button_event : public avo_event
	{
	public:
		mouse_code get_mouse_button() const { return m_button; }

		EVENT_CLASS_CATEGORY(
			event_category_mouse |
			event_category_input |
			event_category_mouse_button)

	protected:
		mouse_button_event(mouse_code button)
			: m_button(button) {}

		mouse_code m_button;
	};

	class mouse_button_pressed_event : public mouse_button_event
	{
	public:
		mouse_button_pressed_event(mouse_code button)
			: mouse_button_event(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_button_pressed_event: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_button_pressed)
	};

	class mouse_button_released_event : public mouse_button_event
	{
	public:
		mouse_button_released_event(mouse_code button)
			: mouse_button_event(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_button_released_event: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_button_released)
	};
}