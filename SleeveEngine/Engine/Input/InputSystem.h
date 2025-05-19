#pragma once
#include "Math/MathFwd.h"

struct GLFWwindow;
// keep the values to be the same as GLFW's values

constexpr unsigned int ENGINE_KEY_SPACE					= 32;
constexpr unsigned int ENGINE_KEY_APOSTROPHE			= 39;		/*'*/
constexpr unsigned int ENGINE_KEY_COMMA					= 44;		/*,*/
constexpr unsigned int ENGINE_KEY_MINUS					= 45;		/*-*/
constexpr unsigned int ENGINE_KEY_PERIOD				= 46;		/*.*/
constexpr unsigned int ENGINE_KEY_SLASH					= 47;		/*/*/
constexpr unsigned int ENGINE_KEY_SEMICOLON				= 59;		/*;*/
constexpr unsigned int ENGINE_KEY_EQUAL					= 61;		/*=*/
constexpr unsigned int ENGINE_KEY_LEFT_BRACKET			= 91;		/*[*/
constexpr unsigned int ENGINE_KEY_BACKSLASH				= 92;		/*\*/
constexpr unsigned int ENGINE_KEY_RIGHT_BRACKET			= 93;		/*]*/
constexpr unsigned int ENGINE_KEY_GRAVE_ACCENT			= 96;		/*`*/
constexpr unsigned int ENGINE_KEY_WORLD_1				= 161;		/*non-US #1*/
constexpr unsigned int ENGINE_KEY_WORLD_2				= 162;		/*non-US #2*/
													 
constexpr unsigned int ENGINE_KEY_ESCAPE				= 256;
constexpr unsigned int ENGINE_KEY_ENTER					= 257;
constexpr unsigned int ENGINE_KEY_TAB					= 258;
constexpr unsigned int ENGINE_KEY_BACKSPACE				= 259;
constexpr unsigned int ENGINE_KEY_INSERT				= 260;
constexpr unsigned int ENGINE_KEY_DELETE				= 261;
constexpr unsigned int ENGINE_KEY_RIGHT					= 262;
constexpr unsigned int ENGINE_KEY_LEFT					= 263;
constexpr unsigned int ENGINE_KEY_DOWN					= 264;
constexpr unsigned int ENGINE_KEY_UP					= 265;
constexpr unsigned int ENGINE_KEY_PAGE_UP				= 266;
constexpr unsigned int ENGINE_KEY_PAGE_DOWN				= 267;
constexpr unsigned int ENGINE_KEY_HOME					= 268;
constexpr unsigned int ENGINE_KEY_END					= 269;
constexpr unsigned int ENGINE_KEY_CAPS_LOCK				= 280;
constexpr unsigned int ENGINE_KEY_SCROLL_LOCK			= 281;
constexpr unsigned int ENGINE_KEY_NUM_LOCK				= 282;
constexpr unsigned int ENGINE_KEY_PRINT_SCREEN			= 283;
constexpr unsigned int ENGINE_KEY_PAUSE					= 284;
constexpr unsigned int ENGINE_KEY_F1					= 290;
constexpr unsigned int ENGINE_KEY_F2					= 291;
constexpr unsigned int ENGINE_KEY_F3					= 292;
constexpr unsigned int ENGINE_KEY_F4					= 293;
constexpr unsigned int ENGINE_KEY_F5					= 294;
constexpr unsigned int ENGINE_KEY_F6					= 295;
constexpr unsigned int ENGINE_KEY_F7					= 296;
constexpr unsigned int ENGINE_KEY_F8					= 297;
constexpr unsigned int ENGINE_KEY_F9					= 298;
constexpr unsigned int ENGINE_KEY_F10					= 299;
constexpr unsigned int ENGINE_KEY_F11					= 300;
constexpr unsigned int ENGINE_KEY_F12					= 301;
constexpr unsigned int ENGINE_KEY_F13					= 302;
constexpr unsigned int ENGINE_KEY_F14					= 303;
constexpr unsigned int ENGINE_KEY_F15					= 304;
constexpr unsigned int ENGINE_KEY_F16					= 305;
constexpr unsigned int ENGINE_KEY_F17					= 306;
constexpr unsigned int ENGINE_KEY_F18					= 307;
constexpr unsigned int ENGINE_KEY_F19					= 308;
constexpr unsigned int ENGINE_KEY_F20					= 309;
constexpr unsigned int ENGINE_KEY_F21					= 310;
constexpr unsigned int ENGINE_KEY_F22					= 311;
constexpr unsigned int ENGINE_KEY_F23					= 312;
constexpr unsigned int ENGINE_KEY_F24					= 313;
constexpr unsigned int ENGINE_KEY_F25					= 314;
constexpr unsigned int ENGINE_KEY_KP_0					= 320;
constexpr unsigned int ENGINE_KEY_KP_1					= 321;
constexpr unsigned int ENGINE_KEY_KP_2					= 322;
constexpr unsigned int ENGINE_KEY_KP_3					= 323;
constexpr unsigned int ENGINE_KEY_KP_4					= 324;
constexpr unsigned int ENGINE_KEY_KP_5					= 325;
constexpr unsigned int ENGINE_KEY_KP_6					= 326;
constexpr unsigned int ENGINE_KEY_KP_7					= 327;
constexpr unsigned int ENGINE_KEY_KP_8					= 328;
constexpr unsigned int ENGINE_KEY_KP_9					= 329;
constexpr unsigned int ENGINE_KEY_KP_DECIMAL			= 330;
constexpr unsigned int ENGINE_KEY_KP_DIVIDE				= 331;
constexpr unsigned int ENGINE_KEY_KP_MULTIPLY			= 332;
constexpr unsigned int ENGINE_KEY_KP_SUBTRACT			= 333;
constexpr unsigned int ENGINE_KEY_KP_ADD				= 334;
constexpr unsigned int ENGINE_KEY_KP_ENTER				= 335;
constexpr unsigned int ENGINE_KEY_KP_EQUAL				= 336;
constexpr unsigned int ENGINE_KEY_LEFT_SHIFT			= 340;
constexpr unsigned int ENGINE_KEY_LEFT_CONTROL			= 341;
constexpr unsigned int ENGINE_KEY_LEFT_ALT				= 342;
constexpr unsigned int ENGINE_KEY_LEFT_SUPER			= 343;
constexpr unsigned int ENGINE_KEY_RIGHT_SHIFT			= 344;
constexpr unsigned int ENGINE_KEY_RIGHT_CONTROL			= 345;
constexpr unsigned int ENGINE_KEY_RIGHT_ALT				= 346;
constexpr unsigned int ENGINE_KEY_RIGHT_SUPER			= 347;
constexpr unsigned int ENGINE_KEY_MENU					= 348;

constexpr unsigned int KEY_NUM							= 358;

constexpr unsigned int ENGINE_MOUSE_BUTTON_1			= 350;
constexpr unsigned int ENGINE_MOUSE_BUTTON_2			= 351;
constexpr unsigned int ENGINE_MOUSE_BUTTON_3			= 352;
constexpr unsigned int ENGINE_MOUSE_BUTTON_4			= 353;
constexpr unsigned int ENGINE_MOUSE_BUTTON_5			= 354;
constexpr unsigned int ENGINE_MOUSE_BUTTON_6			= 355;
constexpr unsigned int ENGINE_MOUSE_BUTTON_7			= 356;
constexpr unsigned int ENGINE_MOUSE_BUTTON_8			= 357;
constexpr unsigned int ENGINE_MOUSE_BUTTON_LEFT			= ENGINE_MOUSE_BUTTON_1;
constexpr unsigned int ENGINE_MOUSE_BUTTON_RIGHT		= ENGINE_MOUSE_BUTTON_2;
constexpr unsigned int ENGINE_MOUSE_BUTTON_MIDDLE		= ENGINE_MOUSE_BUTTON_3;
constexpr unsigned int MOUSE_BUTTON_OFFSET				= 350;

struct KeyState {
	bool m_wasDownLastFrame = false;
	bool m_isDownThisFrame = false;
};

class InputSystem {
public:
	InputSystem();
	~InputSystem();

	void Initialize();
	void BeginFrame();
	void EndFrame();

	bool WasKeyJustPressed( unsigned int keyCode ) const;
	bool WasKeyJustReleased( unsigned int keyCode ) const;
	bool IsKeyDown( unsigned int keyCode ) const;
	Vec2 GetCursorNormalizedOffset() const;

	static void KeyCallback( GLFWwindow* window, int key, int scanCode, int action, int mods );
	static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );

	KeyState m_keyStates[KEY_NUM];
	Vec2 m_cursorPosDeltaOffset;
	Vec2 m_cursorPosThisFrame;
	bool m_skipCursorDeltaOffsetThisFrame = true;
};