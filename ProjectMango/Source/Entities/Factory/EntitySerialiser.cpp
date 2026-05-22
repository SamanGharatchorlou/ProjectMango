#include "pch.h"
#include "EntitySerialiser.h"

#include "System/Files/JSONParser.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "ECS/Components/ComponentHelpers.h"
#include "System/Window.h"
#include "ComponentAssembler.h"

using namespace rapidjson;

// Writes a single fieldInstance entry into a RapidJSON array
// Mirrors the fieldInstances block your ReadMetaData loops over
static inline void WriteFieldInstance(Document::AllocatorType& alloc, Value& field_array,
    const char* identifier, Value value, const char* ldtk_type)
{
    Value field(kObjectType);
    field.AddMember("__identifier", Value(identifier, alloc), alloc);
    field.AddMember("__type", Value(ldtk_type, alloc), alloc);
    field.AddMember("__value", value, alloc);
    field.AddMember("__tile", Value(kNullType), alloc);
    field.AddMember("defUid", -1, alloc); // unknown at runtime; harmless for re-reading
    field.AddMember("realEditorValues", Value(kArrayType), alloc);
    field_array.PushBack(field, alloc);
}

// Builds a full LDtk entity JSON object from EntityMetaData
// Output matches the schema ReadMetaData expects, so you can
// pass the resulting Value straight back into ReadMetaData
static Value SerializeEntityToLDtk(Document::AllocatorType& alloc, const ECS::EntityMetaData& meta_data)
{
    Value entity(kObjectType);
    const Settings& md = meta_data.data;

    VectorF window_to_level = GameData::Get().window->windowToLevel;

    // --- Identity ---
    const char* id = md.Contains("id") ? md.GetString("id") : "Unknown";
    entity.AddMember("__identifier", Value(id, alloc), alloc);
    entity.AddMember("iid", Value("", alloc), alloc); // generate a UUID if needed

    // --- Size (inverse of level_to_window scale) ---
    VectorF size = md.GetVector("size") * window_to_level;
    entity.AddMember("width", size.x, alloc);
    entity.AddMember("height", size.y, alloc);

    // --- Position (px): undo the offset + scale from ReadMetaData ---
    VectorF world_pos = md.GetVector("position") * window_to_level;
    Value px(kArrayType);
    px.PushBack(world_pos.x, alloc);
    px.PushBack(world_pos.y, alloc);
    entity.AddMember("px", px, alloc);

    // --- Pivot ---
    VectorF pivot = md.Contains("pivot_point") ? md.GetVector("pivot_point") : VectorF(0.5f, 0.5f);
    Value piv(kArrayType);
    piv.PushBack(pivot.x, alloc);
    piv.PushBack(pivot.y, alloc);
    entity.AddMember("__pivot", piv, alloc);

    // --- fieldInstances ---
    Value fields(kArrayType);

    // String fields
    for (auto& [key, val] : md.strings.data)
    {
        // stored as __identifier, not a field
        if (key == "Id")
            continue;

        WriteFieldInstance(alloc, fields, key.c_str(), Value(val.c_str(), alloc), "String");
    }

    // Float/value fields
    for (auto& [key, val] : md.values.data)
        WriteFieldInstance(alloc, fields, key.c_str(), Value(val), "Float");

    // Vector fields (written as Array<Float>)
    for (auto& [key, val] : md.vectors.data)
    {
        if (key == "position" || key == "size" || key == "pivot_point")
            continue;

        Value arr(kArrayType);
        arr.PushBack(val.x, alloc);
        arr.PushBack(val.y, alloc);
        WriteFieldInstance(alloc, fields, key.c_str(), std::move(arr), "Array<Float>");
    }

    entity.AddMember("fieldInstances", fields, alloc);
    return entity;
}

void SaveEntityToJson(const char* filepath, const std::vector<ECS::EntityMetaData>& meta_data)
{
    FILE* f = fopen(filepath, "w");
    if (!f)
        return;

    Document doc;
    doc.SetArray();
    auto& alloc = doc.GetAllocator();

    for (u32 i = 0; i < meta_data.size(); i++)
    {
        rapidjson::Value entityJson = SerializeEntityToLDtk(alloc, meta_data[i]);
        doc.PushBack(entityJson, alloc);
    }

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    fwrite(buffer.GetString(), 1, buffer.GetSize(), f);

    fclose(f);
}

bool LoadEntityFromJson(const char* filepath, std::vector<ECS::EntityMetaData>& metaData)
{
    FILE* f = fopen(filepath, "r");
    if (!f)
        return false;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::vector<char> buf(size + 1);
    fread(buf.data(), 1, size, f);
    fclose(f);
    buf[size] = '\0';

    Document doc;
    doc.Parse(buf.data());
    if (doc.HasParseError())
        return false;

    if (doc.IsArray())
    {
        for (Value& json_value : doc.GetArray())
        {
            metaData.push_back(ECS::EntityMetaData());
            ECS::EntityMetaData& meta_data = metaData.back();
            PopulateMetaData(json_value, meta_data);
        }
    }

    // doc IS the entity object — pass directly to your existing reader
    return true;
}