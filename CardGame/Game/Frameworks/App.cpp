#include "Game/Frameworks/App.h"

#include "Engine/Graphics/Renderer.h"
#include "Engine/Window/Window.h"
#include "Engine/Input/InputSystem.h"

#include "Game/Frameworks/Game.h"
#include "Game/Frameworks/GameCommon.h"

#include <chrono>
#include <thread>

void App::Initialize()
{
	g_mainWindow = new Window();
	g_mainWindow->InitWindow();
	g_theResourceManager = new ResourceManager();

	g_theInput = new InputSystem();
	g_theInput->Initialize();

	g_theRenderer = new Renderer();
	g_theRenderer->Initialize();

	g_defaultFont = g_theResourceManager->GetOrLoadFont( "Data/Fonts/arial.ttf" );

	g_theGame = new Game();
	g_theGame->Initialize();

	//g_mainWindow->SetCursorInputMode( CursorInputMode::OFFSET );
}

void App::Run()
{

	while (!AppShouldQuit()) {
		auto startTime = std::chrono::high_resolution_clock::now();

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

		if (g_theInput->WasKeyJustPressed( ENGINE_KEY_ESCAPE )) {
			m_shouldQuit = true;
		}
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
	delete g_mainWindow;

}

void App::BeginFrame()
{
	Clock::TickSystemClock();
	g_mainWindow->BeginFrame();
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
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
}

bool App::AppShouldQuit() const
{
	return g_mainWindow->IsQuitting() || m_shouldQuit;
}

void App::SetMaxFrameRate( int maxFrameRate )
{
	TARGET_FRAME_TIME_MILLISECONDS = 1000.f / (float)maxFrameRate;
}
