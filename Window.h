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
	int keyStatus;

public:
	Window(int width = 640, int height = 480, const char* title = "Hello.")
	 : window(glfwCreateWindow(width, height, title, NULL, NULL)),
	   scale(100.f),
	   location{ 0.f, 0.f },
	   keyStatus(GLFW_RELEASE)
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

		glfwSetScrollCallback(window, wheel);

        glfwSetKeyCallback(window, keyboard);

		resize(window, width, height);
	}
	
	virtual ~Window()
	{
		glfwDestroyWindow(window);
	}

	explicit operator bool()
	{
        if (keyStatus == GLFW_RELEASE) glfwWaitEvents();
        else glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_RELEASE)
            location[0] -= 2.f / size[0];
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_RELEASE)
            location[0] += 2.f / size[0];

        if (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE)
            location[1] -= 2.f / size[1];
        else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE)
            location[1] += 2.f / size[1];

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			location[0] = static_cast<GLfloat>(x) * 2.f / size[0] - 1.f;
			location[1] = 1.f - static_cast<GLfloat>(y) * 2.f / size[1];
		}

		return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);
		
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

	static void wheel(GLFWwindow* window, double x, double y)
	{
		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

		if (instance != NULL)
		{
			instance->scale +=static_cast<GLfloat>(y);
		}
	}

    static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

        if (instance != NULL)
        {
            instance->keyStatus = action;
        }
    }

	const GLfloat* getSize() const {return size; }

	GLfloat getScale() const {return scale; }

	const GLfloat* getLocation() const {return location; }

};

