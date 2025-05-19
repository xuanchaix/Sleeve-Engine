#pragma once
#include "Core/EngineFwdMinor.h"

enum class CursorInputMode {
	NORMAL, HIDDEN, OFFSET,
};

class Window {
public:
	~Window();
	void InitWindow();
	bool IsQuitting() const;
	void BeginFrame();
	static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );
	GLFWwindow* GetGLFWWindow() const;
	Vec2 GetWindowSize() const;
	void SetCursorInputMode( CursorInputMode newMode );
	CursorInputMode GetCursorInputMode() const;

	inline bool HasFrameBufferResized() const;
	inline void SetFrameBufferResized( bool isResized );

protected:
	GLFWwindow* m_window;
	bool m_framebufferResized = false;
	Vec2 m_windowSize;
	CursorInputMode m_cursorInputMode = CursorInputMode::NORMAL;
};

bool Window::HasFrameBufferResized() const
{
	return m_framebufferResized;
}

void Window::SetFrameBufferResized( bool isResized )
{
	m_framebufferResized = isResized;
}