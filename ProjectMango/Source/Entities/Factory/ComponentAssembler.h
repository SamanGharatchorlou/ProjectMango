#pragma once

#include "System/Files/JSONParser.h"

namespace ECS { struct EntityMetaData; }

void PopulateMetaData(rapidjson::Value& json_value, ECS::EntityMetaData& meta_data);
void PopulateMetaData(ECS::Entity, ECS::EntityMetaData& meta_data);
void PopulateMetaData(const char* id, const RectF& rect, ECS::EntityMetaData& meta_data);

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd);