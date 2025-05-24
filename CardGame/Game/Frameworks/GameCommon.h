#pragma once
#include "Engine/Core/EngineFwdMinor.h"
#include "Engine/Entity/Entity.h"

class App;
class Game;

extern Game* g_theGame;
extern App* g_theApp;
extern Font* g_defaultFont;

constexpr float CardWidth = 66.f;
constexpr float CardHeight = 90.f;
constexpr float CardsInBattleLineHeight = 0.f;
constexpr float CardsInHandsHeight = 30.f;
constexpr float CardsSpacing = 72.f;
constexpr float BattleLineY = 50.f;
constexpr float BattleLineLeftX = -290.f;
constexpr float HandsLineY = 140.f;
constexpr float HandsLineLeftX = -250.f;
constexpr float DeckCenterX = -350.f;
constexpr float DeckCenterY = 150.f;
constexpr uint32_t PerFrameTextVertexCount = 600;
constexpr uint32_t PerFrameTextDataSize = PerFrameTextVertexCount * sizeof( VertexPCU3D );