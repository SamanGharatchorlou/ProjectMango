#pragma once

#include "ECS/Components/Collider.h"


class QuadCollider : public ECS::Collider
{
public:
	QuadCollider() { }
	QuadCollider(Quad2D<float>* quad) : mQuad(quad) { }

	void init(Quad2D<float>* quad) { mQuad = quad; }
	bool intersects(const Collider& collider) const override;

#if TRACK_COLLISIONS
	void renderCollider() override;
#endif

	const Quad2D<float>* mQuad;
};