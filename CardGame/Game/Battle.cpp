#include "Game/Battle.h"
#include "Game/Cards/Card.h"
#include "Game/Frameworks/Game.h"

Battle::Battle( EnemyInfo const& enemyInfo )
{
	(void)enemyInfo;
	CardDefinition const& def = CardDefinition::GetDefinition( "Elf Warrior" );
	m_enemyCardsInDeck.push_back( new Card( def ) );
	m_enemyCardsInDeck.push_back( new Card( def ) );
	m_enemyCardsInDeck.push_back( new Card( def ) );
	m_enemyCardsInDeck.push_back( new Card( def ) );
	m_enemyCardsInDeck.push_back( new Card( def ) );
	m_enemyCardsInDeck.push_back( new Card( def ) );

	m_myCardsInDeck.push_back( new Card( def ) );
	m_myCardsInDeck.push_back( new Card( def ) );
	m_myCardsInDeck.push_back( new Card( def ) );
	m_myCardsInDeck.push_back( new Card( def ) );
	m_myCardsInDeck.push_back( new Card( def ) );
	m_myCardsInDeck.push_back( new Card( def ) );

	for (auto card : m_myCardsInDeck) {
		card->BeginPlay();
	}
	for (auto card : m_enemyCardsInDeck) {
		card->BeginPlay();
	}
}

Battle::~Battle()
{
	for (auto card : m_myCardsInDeck) {
		delete card;
	}
	for (auto card : m_myCardsInWaitingList) {
		delete card;
	}
	for (auto card : m_myCardsInBattleLine) {
		delete card;
	}
	for (auto card : m_enemyCardsInDeck) {
		delete card;
	}
	for (auto card : m_enemyCardsInWaitingList) {
		delete card;
	}
	for (auto card : m_enemyCardsInBattleLine) {
		delete card;
	}
}

void Battle::BeginPlay()
{

}

void Battle::PerformNextStep()
{
	m_readyToPerformNextStep = false;
	if (m_battleState == BattleState::SelfStartTurn) {
		m_battleState = BattleState::SelfDrawCard;
		DrawCard( true );
	}
	else if (m_battleState == BattleState::SelfDrawCard) {
		m_battleState = BattleState::SelfAttacking;
		m_currentAttackingCardIndex = 0;
		CardAttack( true );
	}
	else if (m_battleState == BattleState::SelfAttacking) {
		++m_currentAttackingCardIndex;
		if (m_currentAttackingCardIndex >= m_myCardsInBattleLine.size()) {
			m_battleState = BattleState::SelfPlayCard;
			AutoPlayCards( true );
		}
		else {
			CardAttack( true );
		}
	}
	else if (m_battleState == BattleState::SelfPlayCard) {
		m_battleState = BattleState::SelfEndTurn;
		// remove dead cards
		RemoveDeadCards( true );
	}
	else if (m_battleState == BattleState::SelfEndTurn) {
		m_battleState = BattleState::EnemyStartTurn;
		// reduce the cool down of each card in wait list
		for (auto card : m_enemyCardsInWaitingList) {
			--card->m_curCoolDown;
		}
	}
	else if (m_battleState == BattleState::EnemyStartTurn) {
		m_battleState = BattleState::EnemyDrawCard;
		// draw a card
		DrawCard( false );
	}
	else if (m_battleState == BattleState::EnemyDrawCard) {
		m_battleState = BattleState::EnemyAttacking;
		m_currentAttackingCardIndex = 0;
		CardAttack( false );
	}
	else if (m_battleState == BattleState::EnemyAttacking) {
		++m_currentAttackingCardIndex;
		if (m_currentAttackingCardIndex >= m_enemyCardsInBattleLine.size()) {
			m_battleState = BattleState::EnemyPlayCard;
			AutoPlayCards( false );
		}
		else {
			CardAttack( false );
		}
	}
	else if (m_battleState == BattleState::EnemyPlayCard) {
		m_battleState = BattleState::EnemyEndTurn;
		// remove dead cards
		RemoveDeadCards( false );
	}
	else if (m_battleState == BattleState::EnemyEndTurn) {
		m_battleState = BattleState::SelfStartTurn;
		// reduce the cool down of each card in wait list
		for (auto card : m_myCardsInWaitingList) {
			--card->m_curCoolDown;
		}
	}
}

void Battle::Update( float deltaSeconds )
{
	HandleMouseInput();

	if (m_battleState == BattleState::SelfStartTurn) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::SelfDrawCard) {
		m_timer += deltaSeconds;
		if (m_timer > 0.5f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::SelfAttacking) {
		m_timer += deltaSeconds;
		if (m_timer > 0.5f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::SelfPlayCard) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::SelfEndTurn) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::EnemyStartTurn) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::EnemyDrawCard) {
		m_timer += deltaSeconds;
		if (m_timer > 0.5f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::EnemyAttacking) {
		m_timer += deltaSeconds;
		if (m_timer > 0.5f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::EnemyPlayCard) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}
	else if (m_battleState == BattleState::EnemyEndTurn) {
		m_timer += deltaSeconds;
		if (m_timer > 0.3f) {
			m_timer = 0.f;
			m_readyToPerformNextStep = true;
		}
	}

	UpdateCardPosition();

	for (auto card : m_myCardsInBattleLine) {
		card->Update( deltaSeconds );
	}
	for (auto card : m_enemyCardsInBattleLine) {
		card->Update( deltaSeconds );
	}
	for (auto card : m_myCardsInWaitingList) {
		card->Update( deltaSeconds );
	}
	for (auto card : m_enemyCardsInWaitingList) {
		card->Update( deltaSeconds );
	}
}

void Battle::Render() const
{
	for (auto card : m_myCardsInBattleLine) {
		card->Render();
	}
	for (auto card : m_enemyCardsInBattleLine) {
		card->Render();
	}
	for (auto card : m_myCardsInWaitingList) {
		card->Render();
	}
	for (auto card : m_enemyCardsInWaitingList) {
		card->Render();
	}
}

void Battle::CardAttack( bool isSelf )
{
	if (isSelf) {
		if (m_currentAttackingCardIndex >= m_myCardsInBattleLine.size()) {
			return;
		}
		// have enemy in front of the card
		else if (m_currentAttackingCardIndex < m_enemyCardsInBattleLine.size()) {
			m_enemyCardsInBattleLine[m_currentAttackingCardIndex]->GetDamage( m_myCardsInBattleLine[m_currentAttackingCardIndex]->m_curDamage );
		}
		else {
			m_enemyMorale -= m_myCardsInBattleLine[m_currentAttackingCardIndex]->m_curDamage;
		}
	}
	else {
		if (m_currentAttackingCardIndex >= m_enemyCardsInBattleLine.size()) {
			return;
		}
		else if (m_currentAttackingCardIndex < m_myCardsInBattleLine.size()) {
			m_myCardsInBattleLine[m_currentAttackingCardIndex]->GetDamage( m_enemyCardsInBattleLine[m_currentAttackingCardIndex]->m_curDamage );
		}
		else {
			m_myMorale -= m_enemyCardsInBattleLine[m_currentAttackingCardIndex]->m_curDamage;
		}
	}
}

void Battle::DrawCard( bool isSelf )
{
	if (isSelf) {
		if (m_myCardsInDeck.size() > 0) {
			Card* cardToDraw = m_myCardsInDeck[m_myCardsInDeck.size() - 1];
			m_myCardsInWaitingList.push_back( cardToDraw );
			m_myCardsInDeck.pop_back();
		}
	}
	else {
		if (m_enemyCardsInDeck.size() > 0) {
			Card* cardToDraw = m_enemyCardsInDeck[m_enemyCardsInDeck.size() - 1];
			m_enemyCardsInWaitingList.push_back( cardToDraw );
			m_enemyCardsInDeck.pop_back();
		}
	}
}

void Battle::AutoPlayCards( bool isSelf )
{
	if (isSelf) {
		// check all cards and if there is any card cool down is 0, play them
		for (size_t i = 0; i < m_myCardsInWaitingList.size();) {
			if (m_myCardsInWaitingList[i]->m_curCoolDown == 0) {
				m_myCardsInWaitingList[i]->m_inBattleLine = true;
				m_myCardsInBattleLine.push_back( m_myCardsInWaitingList[i] );
				m_myCardsInWaitingList.erase( m_myCardsInWaitingList.begin() + i );
			}
			else {
				++i;
			}
		}
	}
	else {
		// check all cards and if there is any card cool down is 0, play them
		for (size_t i = 0; i < m_enemyCardsInWaitingList.size();) {
			if (m_enemyCardsInWaitingList[i]->m_curCoolDown == 0) {
				m_enemyCardsInWaitingList[i]->m_inBattleLine = true;
				m_enemyCardsInBattleLine.push_back( m_enemyCardsInWaitingList[i] );
				m_enemyCardsInWaitingList.erase( m_enemyCardsInWaitingList.begin() + i );
			}
			else {
				++i;
			}
		}
	}
}

void Battle::RemoveDeadCards( bool isSelf )
{
	for (size_t i = 0; i < m_myCardsInBattleLine.size();) {
		if (m_myCardsInBattleLine[i]->m_curHealth <= 0) {
			delete m_myCardsInBattleLine[i];
			m_myCardsInBattleLine.erase( m_myCardsInBattleLine.begin() + i );
		}
		else {
			++i;
		}
	}
	for (size_t i = 0; i < m_enemyCardsInBattleLine.size();) {
		if (m_enemyCardsInBattleLine[i]->m_curHealth <= 0) {
			delete m_enemyCardsInBattleLine[i];
			m_enemyCardsInBattleLine.erase( m_enemyCardsInBattleLine.begin() + i );
		}
		else {
			++i;
		}
	}
}

constexpr float cardsInBattleLineHeight = 0.f;
constexpr float cardsInHandsHeight = 30.f;

void Battle::UpdateCardPosition()
{
	for (size_t i = 0; i < m_myCardsInBattleLine.size(); ++i) {
		Card* card = m_myCardsInBattleLine[i];
		Vec2 centerPos = Vec2( -290.f + i * 72.f, -50.f );
		card->m_cardBounds2D = AABB2( centerPos - Vec2( CardWidth * 0.5f, CardHeight * 0.5f ), centerPos + Vec2( CardWidth * 0.5f, CardHeight * 0.5f ) );
		card->m_position = Vec3( -290.f + i * 72.f, -50.f, cardsInBattleLineHeight );
	}
	for (size_t i = 0; i < m_enemyCardsInBattleLine.size(); ++i) {
		Card* card = m_enemyCardsInBattleLine[i];
		Vec2 centerPos = Vec2( -290.f + i * 72.f, 50.f );
		card->m_cardBounds2D = AABB2( centerPos - Vec2( CardWidth * 0.5f, CardHeight * 0.5f ), centerPos + Vec2( CardWidth * 0.5f, CardHeight * 0.5f ) );
		card->m_position = Vec3( -290.f + i * 72.f, 50.f, cardsInBattleLineHeight );
	}
	for (size_t i = 0; i < m_myCardsInWaitingList.size(); ++i) {
		Card* card = m_myCardsInWaitingList[i];
		Vec2 centerPos = Vec2( -255.f + i * 72.f, -140.f );
		card->m_cardBounds2D = AABB2( centerPos - Vec2( CardWidth * 0.5f, CardHeight * 0.5f ), centerPos + Vec2( CardWidth * 0.5f, CardHeight * 0.5f ) );
		card->m_position = Vec3( -255.f + i * 72.f, -140.f, cardsInHandsHeight );
	}
	for (size_t i = 0; i < m_enemyCardsInWaitingList.size(); ++i) {
		Card* card = m_enemyCardsInWaitingList[i];
		Vec2 centerPos = Vec2( -255.f + i * 72.f, 140.f );
		card->m_cardBounds2D = AABB2( centerPos - Vec2( CardWidth * 0.5f, CardHeight * 0.5f ), centerPos + Vec2( CardWidth * 0.5f, CardHeight * 0.5f ) );
		card->m_position = Vec3( -255.f + i * 72.f, 140.f, cardsInHandsHeight );
	}
}

void Battle::HandleMouseInput()
{
	Vec2 normalizedCursorPos = g_theInput->GetCursorNormalizedScreenPos();
	Vec2 mousePosInBattleLine = g_theGame->m_gameDefault3DCamera->TransferScreenPosToZPlane( normalizedCursorPos, cardsInBattleLineHeight );
	Vec2 mousePosInHands = g_theGame->m_gameDefault3DCamera->TransferScreenPosToZPlane( normalizedCursorPos, cardsInHandsHeight );

	for (auto card : m_myCardsInBattleLine) {
		if (card->m_cardBounds2D.IsPointInside( mousePosInBattleLine )) {
			card->m_isHovering = true;
		}
		else {
			card->m_isHovering = false;
		}
	}
	for (auto card : m_enemyCardsInBattleLine) {
		if (card->m_cardBounds2D.IsPointInside( mousePosInBattleLine )) {
			card->m_isHovering = true;
		}
		else {
			card->m_isHovering = false;
		}
	}
	for (auto card : m_myCardsInWaitingList) {
		if (card->m_cardBounds2D.IsPointInside( mousePosInHands )) {
			card->m_isHovering = true;
		}
		else {
			card->m_isHovering = false;
		}
	}
	for (auto card : m_enemyCardsInWaitingList) {
		if (card->m_cardBounds2D.IsPointInside( mousePosInHands )) {
			card->m_isHovering = true;
		}
		else {
			card->m_isHovering = false;
		}
	}
}

