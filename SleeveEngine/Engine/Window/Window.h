#pragma once
#include "Core/EngineFwdMinor.h"

class Window {
public:
	~Window();
	void InitWindow();
	bool IsQuitting() const;
	void BeginFrame();
	static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );
	GLFWwindow* GetGLFWWindow() const;
	Vec2 GetWindowSize() const;

	inline bool HasFrameBufferResized() const;
	inline void SetFrameBufferResized( bool isResized );

protected:
	GLFWwindow* m_window;
	bool m_framebufferResized = false;
	Vec2 m_windowSize;
};

bool Window::HasFrameBufferResized() const
{
	return m_framebufferResized;
}

void Window::SetFrameBufferResized( bool isResized )
{
	m_framebufferResized = isResized;
}