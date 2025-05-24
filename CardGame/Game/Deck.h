#pragma once
#include "Engine/Entity/Entity.h"
class Battle;

class Deck : public Entity3D {
public:
	Deck( Vec3 const& position, Euler const& orientation = Euler() );
	~Deck();

	/// Begin play will be called right after an entity is created
	virtual void BeginPlay() override;
	/// Update will be called each frame
	virtual void Update( float deltaSeconds ) override;
	/// Render will be called each frame
	virtual void Render() const override;

public:
	bool m_isFriendly = false;
	AABB2 m_cardBounds2D;
	Battle* m_battle = nullptr;
protected:
	std::vector<VertexPCU3D> m_textVerts;
	VertexBufferBinding m_textVertexBufferBinding;
	TextureBinding m_fontTextureBinding;
};