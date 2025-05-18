#include "Window/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Window::~Window()
{
	glfwDestroyWindow( m_window );
	glfwTerminate();
}

void Window::InitWindow()
{
	glfwInit();
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	m_window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr );
	glfwSetWindowUserPointer( m_window, this );
	glfwSetFramebufferSizeCallback( m_window, FramebufferResizeCallback );
	glfwSetKeyCallback( m_window, InputSystem::KeyCallback );
	glfwSetCursorPosCallback( m_window, InputSystem::CursorPositionCallback );
	glfwSetMouseButtonCallback( m_window, InputSystem::MouseButtonCallback );
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Window::IsQuitting() const
{
	return glfwWindowShouldClose( m_window );
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::FramebufferResizeCallback( GLFWwindow* window, int width, int height )
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer( window ));
	myWindow->m_framebufferResized = true;
}

GLFWwindow* Window::GetGLFWWindow() const
{
	return m_window;
}
