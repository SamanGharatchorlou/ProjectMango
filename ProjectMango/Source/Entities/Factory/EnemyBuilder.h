#pragma once

namespace ECS { struct EntityMetaData; }

ECS::Entity CreateEnemy(const ECS::EntityMetaData& emd);
ECS::Entity CreateEnemy(const char* enemy_type);