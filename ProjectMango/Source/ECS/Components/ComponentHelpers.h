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
	
	struct EntityMetaData
	{
		// the base name e.g. text, card etc.
		BasicString id;

		// unique name e.g. InventoryCoins_White, used to lookup a specific thing
		BasicString uid;

		// generic type e.g. Rune_Rebound, this would be Rune
		// then id would be RuneRebound
		BasicString type;

		VectorF position;
		VectorF size;

		BasicString spriteId;
		SColour colourMod;

		int PtSize = -1;

		// base indentifier i.e. 'button', 'player'
		std::vector<BasicString> tags;

		bool isButton = false;
		bool center = false;
	};

	Entity CreateEntity(const char* id, bool config_postfix = false);
	Entity CreateEntity(const EntityMetaData& emd);

	const char* GetName(Entity entity);
	const Config* GetConfigFromEntity(Entity entity);

	Entity GetParent(Entity child);
	Entity GetFirstChild(Entity parent);
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
		Entity entity = to.entity;
		to = from;
		to.entity = entity;
	}
}