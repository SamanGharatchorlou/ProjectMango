#pragma once

#include "System/Files/JSONParser.h"

namespace ECS { struct EntityMetaData; }

// Writes the entity JSON to a standalone file
void SaveMetaDataToJson(const char* file_path,  const std::vector<ECS::EntityMetaData>& meta_data);

// Loads a saved entity file back into EntityMetaData
bool LoadMetaDataFromJson(const char* file_path, std::vector<ECS::EntityMetaData>& meta_data);

void PopulateMetaDataFromJson(rapidjson::Value& json_value, ECS::EntityMetaData& meta_data);
void PopulateMetaData(const char* id, const RectF& rect, ECS::EntityMetaData& meta_data);