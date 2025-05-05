#pragma once

//forward declaration
class Renderer;
class App;
class ResourceManager;
struct GLFWwindow;

//basic includes
#include "Core/Color.h"
#include "Core/ResourceManager.h"
#include "Math/MathFwd.h"
#include "Graphics/GraphicsFwdMinor.h"

// global variables
extern Renderer* g_theRenderer;
extern App* g_theApp;
extern ResourceManager* g_theResourceManager;