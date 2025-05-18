#pragma once
#include "Core/EngineFwdMinor.h"

class Window {
public:
	~Window();
	void InitWindow();
	bool IsQuitting() const;
	void PollEvents();
	static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );
	GLFWwindow* GetGLFWWindow() const;

	inline bool HasFrameBufferResized() const;
	inline void SetFrameBufferResized( bool isResized );

protected:
	GLFWwindow* m_window;
	bool m_framebufferResized = false;
};

bool Window::HasFrameBufferResized() const
{
	return m_framebufferResized;
}

void Window::SetFrameBufferResized( bool isResized )
{
	m_framebufferResized = isResized;
}