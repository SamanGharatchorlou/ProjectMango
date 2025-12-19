#pragma once

namespace ECS { struct EntityMetaData; }

namespace Player
{
	ECS::Entity Spawn(const ECS::EntityMetaData& emd);
};