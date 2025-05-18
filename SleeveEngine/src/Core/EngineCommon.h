#pragma once

class Renderer;
class App;
class ResourceManager;
class Clock;
class InputSystem;

// global variables
extern Renderer* g_theRenderer;
extern App* g_theApp;
extern ResourceManager* g_theResourceManager;
extern InputSystem* g_theInput;
extern float TARGET_FRAME_TIME_MILLISECONDS;