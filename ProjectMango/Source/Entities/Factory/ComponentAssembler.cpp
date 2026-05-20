#include "pch.h"
#include "ComponentAssembler.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

static bool AddTransform(Entity entity, const ECS::EntityMetaData& emd)
{
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);

	return true;
}

static bool TryAddSprite(ECS::Entity entity, const ECS::EntityMetaData& emd, bool requires_sprite)
{
	bool has_sprite = emd.data.Contains("Sprite");
	if (!has_sprite && !requires_sprite)
		return false;

	Sprite& sprite = AddComponent(Sprite, entity);

	// we might require a sprite, in which case there's no data so its empty, which is fine
	if(has_sprite)
		sprite.Init(emd.data.GetString("Sprite"));

	Transform* transform = GetComponent(Transform, entity);
	if ( sprite.image.texture && transform && transform->size.isZero() )
		DebugPrint(Warning, "CreateBasicObject - Has Sprite, but has no size");

	sprite.params.renderLayer = RenderLayer::BasicObject;
	sprite.params.colourMod = emd.data.GetColour("Colour");

	// no sprite yet, try get a coloured version
	Colour::Type colour_type = (Colour::Type)emd.data.GetFloat("ColourType", -1.0f);
	if (!sprite.image.texture && colour_type != -1)
	{
		StringBuffer64 coloured_sprite;
		AddColourPostfix(emd.data.GetString("Sprite"), colour_type, coloured_sprite);

		sprite.Init(coloured_sprite.c_str());
	}

	return true;
}

static bool TryAddSpriteSheet(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	bool is_sprite_sheet = emd.data.Contains("SpriteSheetFrames");
	if (!is_sprite_sheet)
		return false;

	if (!HasComponent(Sprite, entity))
		TryAddSprite(entity, emd, true);

	ASSERT(emd.data.GetVector("SpriteSheetFrames").lengthSquared() > 0, 
		"Sprite sheet %d has frames counts == 0 (entity %s)", emd.data.GetString("Sprite"), emd.GetID());

	SpriteSheet& ss = AddComponent(SpriteSheet, entity);
	ss.Init(emd.data.GetVector("SpriteSheetFrames").toInt());

	return true;
}

static bool TryAddAnimator(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	bool is_animator = emd.data.Contains("Animator");
	if (!is_animator)
		return false;

	if (!HasComponent(Sprite, entity))
		TryAddSprite(entity, emd, true);

	Animator& animator = AddComponent(Animator, entity);
	animator.Init(emd);

	AddComponent(EntityState, entity);

	return true;
}

static bool TryAddButton(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	if (emd.data.Contains("ButtonCallback"))
	{
		UIButton& button = GetOrAddComponent(UIButton, entity);
		button.callback = emd.data.GetString("ButtonCallback");

		return true;
	}

	return false;
}

static bool TryAddCallback(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	if (emd.data.Contains("Callback"))
	{
		Callback& cb = GetOrAddComponent(Callback, entity);
		cb.callback = emd.data.GetString("Callback");

		return true;
	}

	return false;
}

static bool TryAddFaction(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	if (emd.data.Contains("Faction"))
	{
		Faction& faction = GetOrAddComponent(Faction, entity);
		faction.team = Faction::GetTeam(emd.data.GetString("Faction"));

		return true;
	}

	return false;
}

static bool TryAddText(ECS::Entity entity, const ECS::EntityMetaData& emd)
{
	bool text_callback = emd.data.Contains("TextCallback");
	bool text = emd.data.Contains("Text");
	if (text_callback || text)
	{
		// UIText
		UIText& ui_text = AddComponent(UIText, entity);
		ui_text.center = emd.data.GetBool("Center");
		ui_text.SetSize(emd.data.GetInt("PtSize"));
		ui_text.SetColour(emd.data.GetColour("Colour"));

		if (text_callback)
		{
			ui_text.callback = emd.data.GetString("TextCallback");
		}

		if (text)
		{
			ui_text.SetText(emd.data.GetString("Text"));
		}

		Colour::Type colour_type = (Colour::Type)emd.data.GetFloat("ColourType", -1.0f);
		if (colour_type != -1)
		{
			Colour& colour = AddComponent(Colour, entity);
			colour.colour = colour_type;
		}

		return true;
	}

	return false;
}

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateEntity(emd);

	AddTransform(entity, emd);

	TryAddSprite(entity, emd, false);
	TryAddSpriteSheet(entity, emd);
	TryAddAnimator(entity, emd);
	TryAddButton(entity, emd);
	TryAddCallback(entity, emd);
	TryAddFaction(entity, emd);
	TryAddText(entity, emd);

	return entity;
}