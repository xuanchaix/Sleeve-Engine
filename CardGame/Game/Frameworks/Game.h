#pragma once
#include "Game/Frameworks/GameCommon.h"

class Card;
class Battle;

enum class GameState {
	InBattle,
};

class Game {
public:
	~Game();
	void Initialize();
	void Update( float deltaSeconds );
	void Render() const;

	PerspectiveCamera* m_gameDefault3DCamera = nullptr;
	OrthographicCamera* m_gameDefault2DCamera = nullptr;

	GameState m_gameState = GameState::InBattle;
	std::vector<Card*> m_myDeck;
	Battle* m_currentBattle;
};