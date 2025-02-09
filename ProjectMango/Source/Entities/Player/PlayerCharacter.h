#pragma once

namespace ECS { struct EntityMetaData; }

namespace Player
{
	ECS::Entity Get();
	ECS::Entity Spawn(const ECS::EntityMetaData& emd);
};