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
	glfwSetMouseButtonCallback( m_window, InputSystem::MouseButtonCallback );

	int width, height;
	glfwGetWindowSize( m_window, &width, &height );
	m_windowSize = Vec2( (float)width, (float)height );
}

bool Window::IsQuitting() const
{
	return glfwWindowShouldClose( m_window );
}

void Window::BeginFrame()
{
	glfwPollEvents();
}

void Window::FramebufferResizeCallback( GLFWwindow* window, int width, int height )
{
	auto myWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer( window ));
	myWindow->m_framebufferResized = true;
	glfwGetWindowSize( window, &width, &height );
	myWindow->m_windowSize = Vec2( (float)width, (float)height );
}

GLFWwindow* Window::GetGLFWWindow() const
{
	return m_window;
}

Vec2 Window::GetWindowSize() const
{
	return m_windowSize;
}

void Window::SetCursorInputMode( CursorInputMode newMode )
{
	if (m_cursorInputMode != newMode) {
		m_cursorInputMode = newMode;
		if (m_cursorInputMode == CursorInputMode::NORMAL) {
			glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
		}
		else if (m_cursorInputMode == CursorInputMode::HIDDEN) {
			glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
		}
		else if (m_cursorInputMode == CursorInputMode::OFFSET) {
			glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
		}
	}
}

CursorInputMode Window::GetCursorInputMode() const
{
	return m_cursorInputMode;
}
