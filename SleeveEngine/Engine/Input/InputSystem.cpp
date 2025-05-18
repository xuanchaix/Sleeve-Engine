#include "Input/InputSystem.h"
#include <GLFW/glfw3.h>

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

void InputSystem::CursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
{
	Vec2 thisFramePos = Vec2( (float)xpos, (float)ypos );
	if (glfwGetInputMode( window, GLFW_CURSOR ) == GLFW_CURSOR_DISABLED) {
		g_theInput->m_cursorPosDeltaOffset = thisFramePos - g_theInput->m_cursorPosThisFrame;
	}
	g_theInput->m_cursorPosThisFrame = thisFramePos;
}

