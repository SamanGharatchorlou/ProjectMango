#include "UIEntityBuilder.h"


#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"
#include "EntityBuilder.h"

using namespace ECS;

typedef Entity(*CreateEntityFn)(const EntityMetaData&);

Entity CreateUICursor()
{
	Entity entity = CreateEntity("Cursor", true);

	const Config* config = GetConfig(entity);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(config, VectorF());
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

static ECS::Entity CreateButton(const ECS::EntityMetaData& emd)
{


	return CreateBasicObject(emd);
}


void CreateUIEntities()
{
	CreateUICursor();

	// map entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	//CreateEntitiyFunctions["Cursor"] = CreatePlayerSpawner;


	CreateEntitiyFunctions["Button"] = CreateButton;
}