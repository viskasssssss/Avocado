#include "pch.h"
#include "windows_window.h"

#include "events/built_in_events.h"

namespace avocado
{
	namespace
	{
		void glfw_window_deleter(GLFWwindow* window)
		{
			if (window)
				glfwDestroyWindow(window);
		}
	}

	static bool s_GLFWInitialized = false;

	static void glfw_error_callback(int error, const char* description)
	{
		AVO_CRITICAL("GLFW Error ({0}) | {1}", error, description);
	}

	ref<window> window::create(const window_props& props)
	{
		return create_ref<windows_window>(props);
	}

	windows_window::windows_window(const window_props& props)
	{
		init(props);
	}

	windows_window::~windows_window() 
	{
		shutdown();
	}

	void windows_window::init(const window_props& props)
	{
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		AVO_INFO("Creating window '{0}' ({1}x{2})", props.title, props.width, props.height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			AVO_ASSERT(success, "Can't init GLFW");
			glfwSetErrorCallback(glfw_error_callback);

			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr), glfw_window_deleter);
		glfwSetWindowUserPointer(m_window.get(), &m_data);
		//set_vsync(true);

		glfwSetWindowSizeCallback(m_window.get(), [](GLFWwindow* window, int width, int height)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				window_resize_event event(width, height);
				data.event_callback(event);
			});

		glfwSetWindowCloseCallback(m_window.get(), [](GLFWwindow* window)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);
				window_close_event event;
				data.event_callback(event);
			});

		glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					key_pressed_event event(key, 0);
					data.event_callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					key_released_event event(key);
					data.event_callback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					key_pressed_event event(key, true);
					data.event_callback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_window.get(), [](GLFWwindow* window, unsigned int keycode)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);

				key_typed_event event(keycode);
				data.event_callback(event);
			});

		glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					mouse_button_pressed_event event(button);
					data.event_callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					mouse_button_released_event event(button);
					data.event_callback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_window.get(), [](GLFWwindow* window, double x_offset, double y_offset)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);

				mouse_scrolled_event event((float)x_offset, (float)y_offset);
				data.event_callback(event);
			});

		glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos)
			{
				window_data& data = *(window_data*)glfwGetWindowUserPointer(window);

				mouse_moved_event event((float)xPos, (float)yPos);
				data.event_callback(event);
			});
	}

	void windows_window::shutdown()
	{
		m_window.reset();
	}

	void windows_window::on_update()
	{
		glfwPollEvents();

		if (m_window && glfwGetWindowAttrib(m_window.get(), GLFW_CLIENT_API) != GLFW_NO_API)
			glfwSwapBuffers(m_window.get());
	}

	void windows_window::set_vsync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_data.vsync = enabled;
	}

	bool windows_window::is_vsync() const
	{
		return m_data.vsync;
	}
}