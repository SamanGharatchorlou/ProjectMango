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

//static ECS::Entity CreateButton(const ECS::EntityMetaData& emd)
//{
//	return CreateBasicObject(emd);
//}
//
//Entity CreateText(const EntityMetaData& emd)
//{
//	return CreateBasicObject(emd);
//}

void CreateUIEntities()
{
	CreateUICursor();
}

//static std::unordered_map<BasicString, CreateEntityFn> s_createEntitiyFunctions;
//
//void CreateUIEntities()
//{
//	s_createEntitiyFunctions.clear();
//
//	// UI entities
//	s_createEntitiyFunctions["Cursor"] = CreateUICursor;
//
//	// these dont seem correct, its not often ill just want to create an entity called button that is a button
//	// is first some other entity, e.g. a card, and i add a button thing to it, to make it also be a button
//	s_createEntitiyFunctions["Text"] = CreateText;
//	s_createEntitiyFunctions["Button"] = CreateButton;
//}

//// accessors
//bool IsUIEntity(const char* entity_type)
//{
//	return s_createEntitiyFunctions.contains(entity_type);
//}
//ECS::Entity CreateUIEntity(const char* entity_type, const ECS::EntityMetaData& emd)
//{
//	return s_createEntitiyFunctions.at(entity_type)(emd);
//}