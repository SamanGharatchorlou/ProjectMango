#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"
#include "EntityBuilder.h"

#include "Entities/CardRegistry.h"

using namespace ECS;

typedef Entity(*CreateEntityFn)(const EntityMetaData&);

Entity CreateUICursor()
{
	Entity entity = CreateEntity("Cursor", true);

	const Config* config = GetConfigFromEntity(entity);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.size = config->data.GetVectorF("size_x", "size_y");
	transform.SetWorldPosition(VectorF());
	transform.renderOffset = transform.size * -0.5;

	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.renderLayer = RenderLayer::Top;
	sprite.canFlip = false;
	sprite.Init(config);

	UICursor& cursor = AddComponent(UICursor, entity);
	InputManager* input = InputManager::Get();
	cursor.cursor = &input->mCursor;

	return entity;
}

Entity CreateUIText(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd.id.c_str());

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.size = emd.size;
	transform.SetWorldPosition( emd.position - (emd.size * emd.pivotPoint) );
	
	// UIText
	UIText& ui_text = AddComponent(UIText, entity);
	ui_text.UID = emd.uid;
	ui_text.font.Resize(emd.PtSize);
	ui_text.center = emd.center;
	ui_text.SetColour(emd.colourMod);

	return entity;
}

Entity CreateUIButton(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject(emd, false);
	return entity;
}

Entity CreateCardEntity(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd, false );

	Card& card = AddComponent(Card, entity);

	if(emd.colourMod.GetHex() != Colour::s_defaultNoColour )
	{
		// build an empty card (figure out a better way to do this) or just not at all
		// in fact it doesnt need to actually be a card at all, just a sprite
		Sprite& sprite = GetComponentRef(Sprite, card.entity);
		sprite.colourMod = emd.colourMod;
	}
	else
	{
		int index = CardRegistry::PickRandomIndex(emd.tier);
		CardRegistry::GetCard(card, index);
	}

	return entity;
}

void CreateUIEntities()
{
	CreateUICursor();
}
