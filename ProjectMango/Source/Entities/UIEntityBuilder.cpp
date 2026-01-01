#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
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
	transform.size = config->data.GetVector("size");
	transform.SetWorldPosition(VectorF());

	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.params.renderLayer = RenderLayer::Top;
	sprite.Init(nullptr);
	sprite.params.renderOffset = transform.size * -0.5;

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
	ui_text.center = emd.center;
	ui_text.SetSize(emd.PtSize);
	ui_text.SetColour(emd.colourMod);

	if(emd.colourType != -1)
	{
		Colour& colour = AddComponent(Colour, entity);
		colour.colour = (Colour::Type)emd.colourType;
	}

	return entity;
}

Entity CreateCardEntity(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );
	CardRegistry::DrawRandomCard(entity, emd.tier);

	return entity;
}

void CreateUIEntities()
{
	CreateUICursor();
}
