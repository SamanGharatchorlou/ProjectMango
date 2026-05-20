#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Input/InputManager.h"
#include "EntityBuilder.h"
#include "Entities/Registries/CardRegistry.h"

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

static bool RequiresUIText(const EntityMetaData& emd)
{
	return emd.data.Contains("TextCallback") || emd.data.Contains("Text");
}

Entity CreateUIText(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd.GetID());

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);
	
	// UIText
	UIText& ui_text = AddComponent(UIText, entity);
	ui_text.callback = emd.data.GetString("TextCallback");
	ui_text.center = emd.data.GetBool("Center");
	ui_text.SetSize(emd.data.GetInt("PtSize"));
	ui_text.SetColour(emd.data.GetColour("Colour"));

	// todo: do i need this? should usually be a text callbacl?
	if (emd.data.GetString("Text"))
	{
		ui_text.SetText(emd.data.GetString("Text"));
	}

	Colour::Type colour_type = (Colour::Type)emd.data.GetFloat("ColourType" , -1.0f);
	if(colour_type != -1)
	{
		Colour& colour = AddComponent(Colour, entity);
		colour.colour = colour_type;
	}

	return entity;
}

void CreateUIEntities()
{
	CreateUICursor();
}
