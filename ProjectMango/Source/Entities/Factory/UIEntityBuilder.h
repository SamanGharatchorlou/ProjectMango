#pragma once

namespace ECS
{
	struct EntityMetaData;
}

void CreateUIEntities();

ECS::Entity CreateUIText(const ECS::EntityMetaData& emd);
ECS::Entity CreateUIButton(const ECS::EntityMetaData& emd);
