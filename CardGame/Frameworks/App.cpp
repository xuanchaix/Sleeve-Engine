#include "CardGame/Frameworks/App.h"

#include "Engine/Graphics/Renderer.h"
#include "Engine/Window/Window.h"
#include "Engine/Input/InputSystem.h"

#include "CardGame/Frameworks/Game.h"
#include "CardGame/Frameworks/GameCommon.h"

#include <chrono>
#include <thread>

void App::Initialize()
{
	g_window = new Window();
	g_window->InitWindow();
	g_theResourceManager = new ResourceManager();

	g_theInput = new InputSystem();
	g_theInput->Initialize();

	g_theRenderer = new Renderer();
	g_theRenderer->Initialize();

	g_theGame = new Game();
	g_theGame->Initialize();
}

void App::Run()
{
	while (!g_window->IsQuitting()) {
		auto startTime = std::chrono::high_resolution_clock::now();

		g_window->PollEvents();
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
		//printf( "FPS: %.3f\n", 1000.f / frameTime );
	}
}

void App::Exit()
{
	g_theRenderer->WaitForCleanup();
	delete g_theGame;
	delete g_theResourceManager;
	g_theRenderer->Cleanup();
	delete g_theRenderer;
	delete g_theInput;
	delete g_window;

}

void App::BeginFrame()
{
	Clock::TickSystemClock();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
}

void App::RunFrame()
{
	g_theGame->Update( (float)Clock::GetSystemClock()->GetDeltaSeconds() );
	g_theGame->Render();
}

void App::EndFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->EndFrame();
}

void App::SetMaxFrameRate( int maxFrameRate )
{
	TARGET_FRAME_TIME_MILLISECONDS = 1000.f / (float)maxFrameRate;
}
