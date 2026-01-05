#pragma once

namespace ECS
{
	// types
	static u64 archetypeBit(ECS::Component::Type type)
	{
		return (u64)1 << type;
	}

	// directions
	enum Direction { Up, Right, Down, Left, Count };

	static const VectorI s_directions[Direction::Count] 
	{ 
		VectorI(0,-1), VectorI( 1, 0), VectorI(0, 1), VectorI(-1, 0)
	};

	// rendering
	enum class RenderLayer
	{
		None = -1,
		Bottom = 0,
		Scenery = 1,
		Spell = 4,
		Characters = 5,
		BasicObject = 6,
		UI = 8,
		Top = 9,
		Count = 10
	};

	struct Action
	{
		enum Enum
		{
			None,

			Active,
			Inactive,

			Open,
			Close,

			Idle,
			Walk,
			Run,
	
			Fall,
			Jump,
			Hover,

			Roll,
			Crouch,

			AttackWindUp,
			BasicAttack,
			FollowUpAttack,
			AttackRecovery,

			Hurting,
			TakeHit,
			Death,

			Spawning,

			Count
		};
	};

	Action::Enum StringToAction(const char* action);
	const char* ActionToString(Action::Enum action);

	// this is the same as a config, guess its still better to keep them different
	struct EntityMetaData
	{
		const char* GetID() const;
		Settings data;
	};

	Entity CreateEntity(const char* id, bool config_postfix = false);
	Entity CreateEntity(const EntityMetaData& emd);

	const char* GetName(Entity entity);
	const Config* GetConfigFromEntity(Entity entity);

	Entity GetParent(Entity child);
	void GetChildren(Entity parent, std::vector<Entity>& children);
	Entity GetFirstChild(Entity parent);
	void DestroyChildren(Entity parent);
	
	void SetWorldPosition(ECS::Entity entity, VectorF pos);
	VectorF GetPosition(Entity entity);
	RectF GetRect(Entity entity);
	bool GetRotationParams(Entity entity, VectorF& out_aboutPoint, float& out_rotation);

	// facing direction
	SDL_RendererFlip GetFacingDirection(Entity entity);
	VectorI GetFacingDirectionVector(Entity entity);
	VectorI FacingDirectionToVector(SDL_RendererFlip facing);

	void SetFacingDirection(Entity entity, SDL_RendererFlip direction);
	SDL_RendererFlip FlipFacingDirection(Entity entity);
	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target);

	template<class T>
	void CopyComponent( T& to, const T& from )
	{
		Entity to_entity = to.entity;
		to = from;
		to.entity = to_entity;
	}

	struct Level;
	struct Transform;

	bool IsInLevel(const Level& level, const Transform& transform);
	void GetEntitiesInLevel(const Level& level, const std::unordered_map<Entity, u32>& in_entities, std::vector<Entity>& out_entities);
}