#pragma once
#include "Core/EngineCommon.h"

class App {
public:
	void Initialize();
	void Run();
	void Exit();

	void BeginFrame();
	void RunFrame();
	void EndFrame();

	inline bool HasFrameBufferResized() const;
	inline void SetFrameBufferResized( bool isResized );
protected:
	void InitWindow();
	static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );
	bool m_framebufferResized = false;
public:
	GLFWwindow* m_window;
};

bool App::HasFrameBufferResized() const
{
	return m_framebufferResized;
}

void App::SetFrameBufferResized( bool isResized )
{
	m_framebufferResized = isResized;
}