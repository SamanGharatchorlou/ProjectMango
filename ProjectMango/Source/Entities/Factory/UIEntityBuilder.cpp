#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Input/InputManager.h"

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

