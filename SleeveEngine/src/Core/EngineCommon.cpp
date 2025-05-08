#include "Core/EngineCommon.h"
#include "Core/Clock.h"

Renderer* g_theRenderer = nullptr;
App* g_theApp = nullptr;
ResourceManager* g_theResourceManager = nullptr;
float TARGET_FRAME_TIME_MILLISECONDS = 1000.f / 200.f;