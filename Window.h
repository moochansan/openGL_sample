#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* const window;
	GLfloat size[2];
	GLfloat scale;
	GLfloat location[2];

public:
	Window(int width = 640, int height = 480, const char* title = "Hello.")
	 : window(glfwCreateWindow(width, height, title, NULL, NULL)),
	   scale(100.f),
	   location{ 0.f, 0.f }
	{
		if (window == NULL)
		{
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Can't initialize GLEW." << std::endl;
			exit(1);
		}

		glfwSwapInterval(1);
		
		glfwSetWindowUserPointer(window, this);

		glfwSetWindowSizeCallback(window, resize);

		resize(window, width, height);
	}
	
	virtual ~Window()
	{
		glfwDestroyWindow(window);
	}

	explicit operator bool()
	{
		glfwWaitEvents();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			location[0] = static_cast<GLfloat>(x) * 2.f / size[0] - 1.f;
			location[1] = 1.f - static_cast<GLfloat>(y) * 2.f / size[1];
		}

		return !glfwWindowShouldClose(window);
		
	}

	void swapBuffers() const
	{
		glfwSwapBuffers(window);
	}

	static void resize(GLFWwindow* const window, int width, int height)
	{
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		glViewport(0, 0, fbWidth, fbHeight);

		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

		if (instance != NULL)
		{
			instance->size[0] = static_cast<GLfloat>(width);
			instance->size[1] = static_cast<GLfloat>(height);
		}
	}

	const GLfloat* getSize() const {return size; }

	GLfloat getScale() const {return scale; }

	const GLfloat* getLocation() const {return location; }

};

