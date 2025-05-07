#include "Core/App.h"
#include "Graphics/Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Entity/Entity.h"
#include <chrono>

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
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>( currentTime - startTime ).count();
		std::cout << "Time: " << time << std::endl;
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
	g_theRenderer->BeginFrame();
	for (int i = 0; i < 1000; ++i) {
		delete entities[i];
	}
	for (int i = 0; i < 1000; ++i) {
		Entity3D* newEntity = new Entity3D( Vec3( 0.f, 0.5f * (float)i, -0.5f * (float)i ) );
		entities[i] = newEntity;
	}
	for (int i = 0; i < 1000; ++i) {
		entities[i]->BeginPlay();
	}
}

void App::RunFrame()
{
	for (auto entity : entities) {
		entity->Update( 0.1f );
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

