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

		VectorF targetPosition;
		VectorF position;

		VectorF size;
		// rect of the actual object
		//RectF rect;
		//VectorF positionOffset;

		void SetPosition(VectorF pos) 
		{
			targetPosition = pos;
			position = pos;
		}
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		RectF subRect;
		VectorF renderSize;

		Texture* texture = nullptr;
		
		VectorF flipPoint = VectorF(0.5f, 0.5f);
		SDL_RendererFlip flip;

		u32 renderLayer = 0;
		
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