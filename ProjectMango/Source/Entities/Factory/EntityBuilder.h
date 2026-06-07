#pragma once

using namespace ECS;

namespace ECS { struct EntityMetaData; }

void InitEntityFunctions();
void BuildBiomeEntities(Entity& biome);
Entity CreateEntityFromData(const EntityMetaData& meta_data);

Entity CreateBasicObject(const EntityMetaData& emd);
Entity CreateBasicObject(const char* id, VectorF size);
Entity CreateActor(const EntityMetaData& emd, const char* id_override);

Entity CreateCardSpell(const EntityMetaData& emd, Entity card_entity);
Entity CreateMonster(const EntityMetaData& emd);
Entity CreateVFX(const char* vfx, const RectF& rect);
