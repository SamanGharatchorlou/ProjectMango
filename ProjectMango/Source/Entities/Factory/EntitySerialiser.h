#pragma once

namespace ECS { struct EntityMetaData; }

// Writes the entity JSON to a standalone file
void SaveEntityToJson(const char* file_path,  const std::vector<ECS::EntityMetaData>& meta_data);

// Loads a saved entity file back into EntityMetaData
bool LoadEntityFromJson(const char* file_path, std::vector<ECS::EntityMetaData>& meta_data);