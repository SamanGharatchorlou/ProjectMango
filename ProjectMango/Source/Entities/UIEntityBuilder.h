#pragma once

namespace ECS
{
	struct EntityMetaData;
}

void CreateUIEntities();

ECS::Entity CreateUIText(const ECS::EntityMetaData& emd);
ECS::Entity CreateUIButton(const ECS::EntityMetaData& emd);
//ECS::Entity CreateUICheckbox(const ECS::EntityMetaData& emd);
ECS::Entity CreateCardEntity(const ECS::EntityMetaData& emd);


ECS::Entity RecreateCardFromCard(ECS::Entity previous_Card);
