#pragma once
#include "Game/Frameworks/GameCommon.h"

class Game {
public:
	~Game();
	void Initialize();
	void Update( float deltaSeconds );
	void Render() const;

	PerspectiveCamera* m_gameDefault3DCamera = nullptr;
};