#include "CardGame/Frameworks/Game.h"
#include "CardGame/Frameworks/GameCommon.h"
#include "Engine/Graphics/Renderer.h"
#include <vector>

std::vector<Entity3D*> entities;

Game::~Game()
{
	for (auto entity : entities) {
		delete entity;
	}
	delete m_gameDefault3DCamera;
}

void Game::Initialize()
{
	m_gameDefault3DCamera = new PerspectiveCamera();
	m_gameDefault3DCamera->BeginPlay();
	m_gameDefault3DCamera->m_aspect = g_theRenderer->GetSwapChainExtentRatio();
	m_gameDefault3DCamera->m_fov = 60.f;
	m_gameDefault3DCamera->m_zNear = 0.1f;
	m_gameDefault3DCamera->m_zFar = 100.f;
	m_gameDefault3DCamera->m_position = Vec3( 0.f, -1.f, 1.73f );
	m_gameDefault3DCamera->m_orientation = Euler( 90.f, 60.f, 0.f );

	for (int i = 0; i < 1000; ++i) {
		Entity3D* newEntity = new Entity3D( Vec3( 0.f, 0.5f * (float)i, -0.5f * (float)i ) );
		entities.push_back( newEntity );
		newEntity->BeginPlay();
	}
}

void Game::Update( float deltaSeconds )
{
	for (auto entity : entities) {
		entity->Update(deltaSeconds);
	}
}

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameDefault3DCamera );
	for (auto entity : entities) {
		entity->Render();
	}
	g_theRenderer->EndCamera( m_gameDefault3DCamera );
}
