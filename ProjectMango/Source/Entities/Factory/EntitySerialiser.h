#pragma once


#include "System/Files/JSONParser.h"
//#include "ECS/Components/ComponentHelpers.h"

namespace ECS { struct EntityMetaData; }

struct LevelSizeInfo
{
    VectorF windowToLevel;
    VectorF LevelWorldPos;
};

void ReadMetaDataFromJson(rapidjson::Value& json_value, LevelSizeInfo size_Info, ECS::EntityMetaData& meta_data);

void ReadMetaDataFromEntity(ECS::Entity, ECS::EntityMetaData& meta_data);

// Writes the entity JSON to a standalone file
// The file can be read back with your existing ReadMetaData by
// loading the root object directly as the `data_in` argument
void SaveEntityToJson(const char* file_path, 
    const LevelSizeInfo& size_Info, const std::vector<ECS::EntityMetaData>& meta_data);

// Loads a saved entity file back into EntityMetaData
// Just a thin wrapper so call sites are symmetric with SaveEntityToFile
bool LoadEntityFromJson(const char* file_path,
    const LevelSizeInfo& size_Info, std::vector<ECS::EntityMetaData>& meta_data);