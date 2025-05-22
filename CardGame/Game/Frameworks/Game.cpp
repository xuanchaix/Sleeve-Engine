#include "Game/Frameworks/Game.h"
#include "Game/Frameworks/GameCommon.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Window/Window.h"
#include "Game/Cards/Card.h"
#include "Game/Battle.h"
#include "Game/EnemyInfo.h"
#include <vector>

std::vector<EntityBase*> entities;

Game::~Game()
{
	delete m_currentBattle;
	for (auto entity : entities) {
		delete entity;
	}
	delete m_gameDefault3DCamera;
	delete m_gameDefault2DCamera;
}

void Game::Initialize()
{
	CardDefinition::SetUpCardDefinitions();
	m_gameDefault3DCamera = new PerspectiveCamera();
	m_gameDefault3DCamera->BeginPlay();
	m_gameDefault3DCamera->m_aspect = g_theRenderer->GetSwapChainExtentRatio();
	m_gameDefault3DCamera->m_fov = 60.f;
	m_gameDefault3DCamera->m_zNear = 0.1f;
	m_gameDefault3DCamera->m_zFar = 1000.f;
	m_gameDefault3DCamera->m_position = Vec3( 0.f, 0.f, 350.f );
	m_gameDefault3DCamera->m_orientation = Euler( 90.f, 90.f, 0.f );

	m_gameDefault2DCamera = new OrthographicCamera();
	m_gameDefault2DCamera->BeginPlay();
	m_gameDefault2DCamera->m_near = 1.f;
	m_gameDefault2DCamera->m_far = -1.f;
	m_gameDefault2DCamera->m_bounds = AABB2( Vec2( 0.f, 0.f ), Vec2( 1600.f, 800.f ) );


	EnemyInfo info;
	m_currentBattle = new Battle( info );
}

void Game::Update( float deltaSeconds )
{
	// test code
// 	Vec3 fwdVec, leftVec, UpVec;
// 	m_gameDefault3DCamera->m_orientation.GetForwardAndLeftAndUpVector( fwdVec, leftVec, UpVec );
// 	if (g_theInput->IsKeyDown( 'W' )) {
// 		m_gameDefault3DCamera->m_position += fwdVec * deltaSeconds;
// 	}
// 	if (g_theInput->IsKeyDown( 'A' )) {
// 		m_gameDefault3DCamera->m_position += leftVec * deltaSeconds;
// 	}
// 	if (g_theInput->IsKeyDown( 'S' )) {
// 		m_gameDefault3DCamera->m_position -= fwdVec * deltaSeconds;
// 	}
// 	if (g_theInput->IsKeyDown( 'D' )) {
// 		m_gameDefault3DCamera->m_position -= leftVec * deltaSeconds;
// 	}
// 
// 	Vec2 cursorOffset = g_theInput->GetCursorNormalizedOffset();
// 	m_gameDefault3DCamera->m_orientation.yaw -= cursorOffset.x * 0.075f * g_mainWindow->GetWindowSize().x;
// 	m_gameDefault3DCamera->m_orientation.pitch += cursorOffset.y * 0.075f * g_mainWindow->GetWindowSize().y;
// 
// 	m_gameDefault3DCamera->m_orientation.pitch = GetClamped( m_gameDefault3DCamera->m_orientation.pitch, -85.f, 85.f );

	if (m_gameState == GameState::InBattle) {
		m_currentBattle->Update( deltaSeconds );
		if (m_currentBattle->m_readyToPerformNextStep) {
			m_currentBattle->PerformNextStep();
		}
	}

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
	if (m_gameState == GameState::InBattle) {
		m_currentBattle->Render();
	}
	g_theRenderer->EndCamera( m_gameDefault3DCamera );
	g_theRenderer->BeginCamera( m_gameDefault2DCamera );

	g_theRenderer->EndCamera( m_gameDefault2DCamera );
}
