#pragma once

class Renderer;
class App;
class ResourceManager;
class Clock;

// global variables
extern Renderer* g_theRenderer;
extern App* g_theApp;
extern ResourceManager* g_theResourceManager;
extern float TARGET_FRAME_TIME_MILLISECONDS;