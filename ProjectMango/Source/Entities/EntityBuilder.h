#pragma once

namespace ECS { struct EntityMetaData; }

typedef ECS::Entity(*CreateEntityFn)(const ECS::EntityMetaData& emd);

void CreateEntities(ECS::Entity& biome);

ECS::Entity CreateBasicObject(const ECS::EntityMetaData& emd, bool has_config = true);
ECS::Entity CreateBasicObject(const char* id, VectorF size);
