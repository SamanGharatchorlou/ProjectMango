#pragma once

namespace ECS
{
	struct EntityMetaData;
}

void CreateUIEntities();

ECS::Entity CreateCardEntity(const ECS::EntityMetaData& emd);
ECS::Entity RecreateCardFromCard(ECS::Entity previous_Card);
