#include "Core/EngineCommon.h"
#include "Core/Clock.h"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
Window* g_window = nullptr;
ResourceManager* g_theResourceManager = nullptr;
float TARGET_FRAME_TIME_MILLISECONDS = 1000.f / 200.f;