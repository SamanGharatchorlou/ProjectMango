#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/ComponentHelpers.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"
#include "Entities/Factory/ComponentAssembler.h"

using namespace ECS;

typedef Entity(*CreateEntityFn)(const EntityMetaData&);

Entity CreateUICursor()
{
	const Config* config = GetConfig("cursor");

	EntityMetaData meta_data;
	meta_data.data.Merge(config->data);
	meta_data.data.AddVectorF("render_offset", config->data.GetVector("size") * -0.5f);
	Entity entity = AssembleEntity(meta_data);

	UICursor& cursor = AddComponent(UICursor, entity);
	cursor.cursor = &InputManager::Get()->mCursor;

	return entity;
}

