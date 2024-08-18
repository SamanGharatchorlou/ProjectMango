#pragma once

#include "Core/TypeDefs.h"
#include "Core/Vector2D.h"
#include "Core/Rect.h"
#include "Core/stack.h"

#include "Characters/States/CharacterAction.h"

class Texture;
enum class ActionState;

namespace ECS
{
	enum Direction { Up, Right, Down, Left, Count };

	static const VectorI s_directions[Direction::Count] 
	{ 
		VectorI(0,-1), VectorI( 1, 0), VectorI(0, 1), VectorI(-1, 0) 
	};

	struct Transform
	{
		COMPONENT_TYPE(Transform)

		VectorF position;
		VectorF prevPosition;

		// rect of the actual object
		//RectF rect;
		//VectorF positionOffset;

		
		//VectorF forward;
		//VectorF back;

		//VectorF GetPos() const { return rect.Center() + positionOffset; }
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		VectorF renderSize;
		RectF subRect;
		Texture* texture = nullptr;
		u32 renderLayer = 0;
		
		SDL_RendererFlip flip;
	};

	struct CharacterState
	{
		COMPONENT_TYPE(CharacterState)

		ActionStack<CharacterAction> actions;

		VectorI movementInput;
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		Entity target = ECS::EntityInvalid;

		VectorI currentStart;
		VectorI currentTarget;

		std::vector<VectorI> path;

		void UpdateTargetPosition();
	};

	struct Damage
	{
		COMPONENT_TYPE(Damage)

		float value = -1;
	};

	struct Health
	{
		COMPONENT_TYPE(Health)

		float maxHealth = 0.0f;
		float currentHealth = 0.0f;

		bool invulnerable = false;

		std::vector<Entity> ignoredDamaged;

		void ApplyDamage(const Damage& damage);
	};

	// ----------------------------------------------------------------------
	// helpers
	static u64 archetypeBit(ECS::Component::Type type)
	{
		return (u64)1 << type;
	}
}