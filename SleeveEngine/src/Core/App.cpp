#include "Core/App.h"
#include "Graphics/Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Entity/Entity.h"
#include <chrono>
#include <thread>

std::vector<Entity3D*> entities;
extern PerspectiveCamera* globalCamera;

void App::Initialize()
{
	InitWindow();
	g_theResourceManager = new ResourceManager();
	globalCamera = new PerspectiveCamera();
	g_theRenderer = new Renderer();
	g_theRenderer->InitVulkan();
	globalCamera->BeginPlay();
	for (int i = 0; i < 1000; ++i) {
		Entity3D* newEntity = new Entity3D( Vec3( 0.f, 0.5f * (float)i, -0.5f * (float)i ) );
		entities.push_back( newEntity );
		newEntity->BeginPlay();
	}
}

void App::Run()
{
	while (!glfwWindowShouldClose( m_window )) {
		auto startTime = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		BeginFrame();
		RunFrame();
		EndFrame();

		// fit the max frame rate
		auto currentTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>( currentTime - startTime ).count();
		while (frameTime < TARGET_FRAME_TIME_MILLISECONDS) {
			std::this_thread::yield();
			currentTime = std::chrono::high_resolution_clock::now();
			frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>( currentTime - startTime ).count();
		} 
		std::cout << "Time: " << frameTime << std::endl;
	}
}

void App::Exit()
{
	g_theRenderer->WaitForCleanup();
	for (auto entity : entities) {
		delete entity;
	}
	delete globalCamera;
	delete g_theResourceManager;
	g_theRenderer->Cleanup();
	delete g_theRenderer;

	glfwDestroyWindow( m_window );
	glfwTerminate();
}

void App::BeginFrame()
{
	Clock::TickSystemClock();
	g_theRenderer->BeginFrame();
}

void App::RunFrame()
{
	for (auto entity : entities) {
		entity->Update( (float)Clock::GetSystemClock()->GetDeltaSeconds() );
	}
	g_theRenderer->BeginCamera( *globalCamera );
	for (auto entity : entities) {
		entity->Render();
	}
	g_theRenderer->EndCamera( *globalCamera );
}

void App::EndFrame()
{
	g_theRenderer->EndFrame();
}

void App::SetMaxFrameRate( int maxFrameRate )
{
	TARGET_FRAME_TIME_MILLISECONDS = 1000.f / maxFrameRate;
}

void App::InitWindow()
{
	glfwInit();
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	m_window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr );
	glfwSetWindowUserPointer( m_window, this );
	glfwSetFramebufferSizeCallback( m_window, FramebufferResizeCallback );
}

void App::FramebufferResizeCallback( GLFWwindow* window, int width, int height )
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer( window ));
	app->m_framebufferResized = true;
}

