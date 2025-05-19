#pragma once

class Renderer;
class App;
class ResourceManager;
class Clock;
class InputSystem;
class Window;

// global variables
extern Renderer* g_theRenderer;
extern App* g_theApp;
extern ResourceManager* g_theResourceManager;
extern InputSystem* g_theInput;
extern Window* g_mainWindow;
extern float TARGET_FRAME_TIME_MILLISECONDS;