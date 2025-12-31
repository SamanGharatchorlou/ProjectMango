#pragma once

using namespace ECS;

namespace ECS { struct EntityMetaData; }

typedef Entity(*CreateEntityFn)(const EntityMetaData& emd);

void CreateEntities(ECS::Entity& biome);

Entity CreateBasicObject(const EntityMetaData& emd);
Entity CreateBasicObject(const char* id, VectorF size);

Entity CreateActor(const EntityMetaData& emd, const char* id_override);
Entity CreateCardActor(const char* monster, Entity card_entity);
Entity CreateMonster(const EntityMetaData& emd);
Entity CreateVFX(const char* vfx, const RectF& rect);
