#pragma once
#include "Engine/Entity/Entity.h"
#include "Game/Frameworks/GameCommon.h"
#include "Engine/Core/XmlUtils.h"

enum class CardRarity {
	Beginning, Common, Rare, Legendary,
};

struct CardDefinition {
	int m_damage = 1;
	int m_health = 1;
	int m_coolDown = 1;

	bool m_enterAbility = false;
	bool m_enterHandAbility = false;
	bool m_deathAbility = false;
	bool m_attackAbility = false;
	uint32_t m_attributesFlags = 0;
	CardRarity m_rarity = CardRarity::Beginning;
	std::string m_effect;
	std::string m_name;
	std::string m_description;

	CardDefinition();
	CardDefinition( XmlElement* xmlIter );
	static void SetUpCardDefinitions();
	static std::vector<CardDefinition> s_definitions;
	static CardDefinition const& GetDefinition( std::string const& name );
};

class Card :public Entity3D {
public:
	Card( CardDefinition const& def, Vec3 const& position = Vec3(), Euler const& orientation = Euler() );
	~Card();
	/// Begin play will be called right after an entity is created
	virtual void BeginPlay() override;
	/// Update will be called each frame
	virtual void Update( float deltaSeconds ) override;
	/// Render will be called each frame
	virtual void Render() const override;

	uint32_t GetDamage( int damage );

	CardDefinition const& m_def;
	int m_curDamage = 1;
	int m_curHealth = 5;
	int m_curCoolDown = 2;
	bool m_inBattleLine = false;
	std::vector<VertexPCU3D> m_textVerts;
	VertexBufferBinding m_textVertexBufferBinding;
	TextureBinding m_fontTextureBinding;
};