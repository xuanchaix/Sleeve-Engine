#pragma once
#include "Game/Frameworks/GameCommon.h"
#include <deque>
class Card;
struct EnemyInfo;

enum class BattleState {
	SelfStartTurn, SelfDrawCard, SelfPlayCard, SelfAttacking, SelfEndTurn,
	EnemyStartTurn, EnemyDrawCard, EnemyPlayCard, EnemyAttacking, EnemyEndTurn,
};

class Battle {
public:
	Battle( EnemyInfo const& enemyInfo );
	~Battle();
	void BeginPlay();
	void PerformNextStep();
	void Update( float deltaSeconds );
	void Render() const;

protected:
	void CardAttack( bool isSelf );
	void DrawCard( bool isSelf );
	void AutoPlayCards( bool isSelf );
	bool BeginPlayCard();
	void RemoveDeadCards( bool isSelf );
	void UpdateCardPosition();

protected:
	void HandleMouseInput();

public:
	bool m_readyToPerformNextStep = false;
	float m_timer = 0.f;
	uint32_t m_turnCounter = 1;
	uint32_t m_currentAttackingCardIndex = -1;
	BattleState m_battleState = BattleState::SelfStartTurn;
	int m_enemyMorale = 30;
	int m_myMorale = 30;
	int m_currentCardToPlayIndex = -1;
	uint32_t m_myBattleLineSizeBeforePlay = -1;
	Card* m_currentCardToPlay = nullptr;
	std::deque<Card*> m_enemyCardsInBattleLine;
	std::deque<Card*> m_myCardsInBattleLine;
	std::vector<Card*> m_enemyCardsInDeck;
	std::vector<Card*> m_myCardsInDeck;
	std::vector<Card*> m_enemyCardsInWaitingList;
	std::vector<Card*> m_myCardsInWaitingList;
};