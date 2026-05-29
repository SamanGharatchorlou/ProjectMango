#pragma once

namespace ECS 
{ 
	struct EntityMetaData;
}

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd, ECS::Entity entity = ECS::EntityInvalid);
void SerialiseEntity(ECS::Entity, ECS::EntityMetaData& out_emd);

void SetParentsAndChildren(const ECS::EntityMetaData& emd);