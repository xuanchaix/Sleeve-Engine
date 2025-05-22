#include "Input/InputSystem.h"
#include <GLFW/glfw3.h>
#include "Window/Window.h"

extern Window* g_mainWindow;
extern InputSystem* g_theInput;

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::Initialize()
{

}

void InputSystem::BeginFrame()
{
	double xPos, yPos;
	glfwGetCursorPos( g_mainWindow->GetGLFWWindow(), &xPos, &yPos );
	Vec2 thisFramePos = Vec2( (float)xPos, (float)yPos );

	Vec2 windowSize = g_mainWindow->GetWindowSize();

	if (g_mainWindow->GetCursorInputMode() == CursorInputMode::OFFSET) {
		if (m_skipCursorDeltaOffsetThisFrame) {
			m_skipCursorDeltaOffsetThisFrame = false;
			g_theInput->m_cursorPosDeltaOffset = Vec2();
		}
		else {
			g_theInput->m_cursorPosDeltaOffset = thisFramePos - g_theInput->m_cursorPosThisFrame;
			g_theInput->m_cursorPosDeltaOffset /= windowSize;
		}
	}
	else {
		g_theInput->m_cursorPosDeltaOffset = Vec2();
	}
	m_cursorPosThisFrame = thisFramePos;
	m_normalizedCursorPosThisFrame = Vec2( m_cursorPosThisFrame.x / windowSize.x, 1.f - m_cursorPosThisFrame.y / windowSize.y );
}

void InputSystem::EndFrame()
{
	for (int i = 0; i < KEY_NUM; ++i) {
		m_keyStates[i].m_wasDownLastFrame = m_keyStates[i].m_isDownThisFrame;
	}
}

bool InputSystem::WasKeyJustPressed( unsigned int keyCode ) const
{
	return m_keyStates[keyCode].m_isDownThisFrame && !m_keyStates[keyCode].m_wasDownLastFrame;
}

bool InputSystem::WasKeyJustReleased( unsigned int keyCode ) const
{
	return !m_keyStates[keyCode].m_isDownThisFrame && m_keyStates[keyCode].m_wasDownLastFrame;
}

bool InputSystem::IsKeyDown( unsigned int keyCode ) const
{
	return m_keyStates[keyCode].m_isDownThisFrame;
}

Vec2 InputSystem::GetCursorNormalizedOffset() const
{
	return m_cursorPosDeltaOffset;
}

Vec2 InputSystem::GetCursorNormalizedScreenPos() const
{
	return m_normalizedCursorPosThisFrame;
}

void InputSystem::KeyCallback( GLFWwindow* window, int key, int scanCode, int action, int mods )
{
	if (action == GLFW_PRESS) {
		g_theInput->m_keyStates[key].m_isDownThisFrame = true;
	}
	else if (action == GLFW_RELEASE) {
		g_theInput->m_keyStates[key].m_isDownThisFrame = false;
	}
}

void InputSystem::MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	if (action == GLFW_PRESS) {
		g_theInput->m_keyStates[button + MOUSE_BUTTON_OFFSET].m_isDownThisFrame = true;
	}
	else if (action == GLFW_RELEASE) {
		g_theInput->m_keyStates[button + MOUSE_BUTTON_OFFSET].m_isDownThisFrame = false;
	}
}


