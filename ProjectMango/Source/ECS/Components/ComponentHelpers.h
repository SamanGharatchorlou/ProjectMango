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
			BasicAttackHold,
			LungeAttack,
			FloorSlam,

			Hurting,
			TakeHit,
			Death,

			Spawning,

			Count
		};
	};

	Action::Enum StringToAction(const char* action);
	const char* ActionToString(Action::Enum action);

	struct EntityMetaData
	{
		// the base name e.g. text, card etc.
		BasicString id;

		// unique name e.g. InventoryCoins_White, used to lookup a specific thing
		BasicString uid;
		BasicString callback;

		VectorF position;
		VectorF size;
		VectorF pivotPoint;
		
		BasicString spriteId;
		BasicString spriteSheetId;
		BasicString animatorId;

		VectorI spriteSheetFrameCounts = VectorI(1,1);
		SColour colourMod;
		int colourType = -1;

		int PtSize = -1;
		int tier = -1;

		// player = 1, enemy = 2
		int faction = 0;

		// base indentifier i.e. 'button', 'player'
		std::vector<BasicString> tags;

		bool isButton = false;
		bool center = false;
		bool random = false;
		bool snapToFloor = false;
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

	void SetFacingDirection(Entity entity, SDL_RendererFlip direction);
	void FlipFacingDirection(Entity entity);
	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target);

	VectorI FacingDirectionToVector(SDL_RendererFlip facing);

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