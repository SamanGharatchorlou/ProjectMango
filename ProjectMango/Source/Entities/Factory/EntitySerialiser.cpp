#include "pch.h"
#include "EntitySerialiser.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "ECS/Components/ComponentHelpers.h"
#include "System/Window.h"
#include "ComponentAssembler.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Core/Helpers.h"
#include "Core/Maths.h"

using namespace rapidjson;
using namespace ECS;

// Writes a single fieldInstance entry into a RapidJSON array
// Mirrors the fieldInstances block your ReadMetaData loops over
static inline void WriteFieldInstance(Document::AllocatorType& alloc, Value& field_array,
    const char* identifier, Value value, const char* ldtk_type)
{
    Value field(kObjectType);
    field.AddMember("__identifier", Value(identifier, alloc), alloc);
    field.AddMember("__type", Value(ldtk_type, alloc), alloc);
    field.AddMember("__value", value, alloc);
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

    u32 iid = md.Contains("iid") ? (u32)md.GetInt("iid") : Maths::GenerateIID();
    entity.AddMember("iid", iid, alloc);

    // --- Children ---
    if (const std::vector<u64>* children_iids = md.GetIntArray("children_iids"))
    {
        Value children(kArrayType);
        for (u32 i = 0; i < children_iids->size(); i++)
        {
            children.PushBack(children_iids->at(i), alloc);
        }
        entity.AddMember("children_iids", children, alloc);
    }

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
    VectorF pivot = md.GetVector("pivot_point");
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
        if (key == "id" || key == "iid")
            continue;

        WriteFieldInstance(alloc, fields, key.c_str(), Value(val.c_str(), alloc), "String");
    }

    // Float/value fields
    for (auto& [key, val] : md.ints.data)
    {
        // stored as iid, not a field
        if (key == "iid")
            continue;

        WriteFieldInstance(alloc, fields, key.c_str(), Value(val), "Int");
    }    
    for (auto& [key, val] : md.floats.data)
    {
        WriteFieldInstance(alloc, fields, key.c_str(), Value(val), "Float");
    }

    // Vector fields (written as Array<Float>)
    for (auto& [key, val] : md.floatArrays.data)
    {
        if (key == "position" || key == "size" || key == "pivot_point")
            continue;

        Value arr(kArrayType);
        arr.PushBack(val[0], alloc);
        arr.PushBack(val[1], alloc);
        WriteFieldInstance(alloc, fields, key.c_str(), std::move(arr), "Array<Float>");
    }

    entity.AddMember("fieldInstances", fields, alloc);
    return entity;
}

void SaveMetaDataToJson(const char* filepath, const std::vector<ECS::EntityMetaData>& meta_data)
{
    FILE* f = fopen(filepath, "w");
    if (!f)
    {
        DebugPrint(Warning, "Invalid file path: cannot save entities to Json %s", filepath);
        return;
    }

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

    DebugPrint(Log, "Success: saved %d entities to file: %s", (int)meta_data.size(), filepath);
}

bool LoadMetaDataFromJson(const char* filepath, std::vector<ECS::EntityMetaData>& metaData)
{
    FILE* f = fopen(filepath, "r");
    if (!f)
    {
        DebugPrint(Warning, "Invalid file path: cannot load entities from Json %s", filepath);
        return false;
    }

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
    {
        DebugPrint(Warning, "Parser error: cannot load entities from Json %s", filepath);
        return false;
    }

    if (doc.IsArray())
    {
        for (Value& json_value : doc.GetArray())
        {
            metaData.push_back(ECS::EntityMetaData());
            ECS::EntityMetaData& meta_data = metaData.back();
            PopulateMetaDataFromJson(json_value, meta_data);
        }
    }

    // doc IS the entity object — pass directly to your existing reader
    return true;
}

void PopulateMetaDataFromJson(Value& jsonValue, EntityMetaData& metaData)
{
    Settings& settings = metaData.data;

    settings.AddString("id", jsonValue["__identifier"].GetString());

    settings.AddU64("iid", Maths::GenerateIID());
    if (jsonValue.HasMember("iid"))
    {
        Value& iid_value = jsonValue["iid"];
        if (iid_value.GetType() == kJsonType::String)
        {
            std::size_t hash = std::hash<BasicString>{ }(iid_value.GetString());
            u64 iid = static_cast<u64>(hash);
            settings.AddU64("iid", iid);
        }
        else if(iid_value.GetType() == kJsonType::Number)
        {
            settings.AddU64("iid", iid_value.GetUint64());
        }
    }

    if (jsonValue.HasMember("children_iids"))
    {
        const Value::Array& children_iids = jsonValue["children_iids"].GetArray();
        for (u32 i = 0; i < children_iids.Size(); i++)
        {
            settings.AddIntArrayMember("children_iids", children_iids[i].GetUint());
        }
    }

    float width = jsonValue["width"].GetFloat();
    float height = jsonValue["height"].GetFloat();

    VectorF window_to_level = GameData::Get().window->windowToLevel;
    ASSERT(!window_to_level.isZero(), "window to level has not been setup yet, cannot init UI");

    VectorF level_to_window;
    level_to_window.x = 1.0f / window_to_level.x;
    level_to_window.y = 1.0f / window_to_level.y;

    Value& px = jsonValue["px"];
    float px_x = px[0].GetFloat(); // + (width * 0.5f);
    float px_y = px[1].GetFloat(); // - (height);
    settings.AddVectorF("position", (VectorF(px_x, px_y) * level_to_window));
    settings.AddVectorF("size", (VectorF(width, height) * level_to_window));

    Value& pivot = jsonValue["__pivot"];
    float pivot_x = pivot[0].GetFloat();
    float pivot_y = pivot[1].GetFloat();
    settings.AddVectorF("pivot_point", VectorF(pivot_x, pivot_y));

    if (jsonValue.HasMember("fieldInstances"))
    {
        const Value::Array& field_instances = jsonValue["fieldInstances"].GetArray();
        for (u32 i = 0; i < field_instances.Size(); i++)
        {
            Value& field_instance = field_instances[i];

            const char* field_id = field_instance["__identifier"].GetString();
            kJsonType field_type = (kJsonType)field_instance["__value"].GetType();
            Value& field_value = field_instance["__value"];

            bool did_populate = PopulateSettingByType(field_value, field_id, field_type, settings);
            if (!did_populate)
            {
                DebugPrint(Warning, "Failed to populate field %s", field_id);
            }
        }

        // custom data
        if (settings.Contains("size_override"))
        {
            VectorF size = settings.GetVector("size_override");
            settings.AddVectorF("size", (size * level_to_window));
        }
        else if (settings.Contains("colour_type"))
        {
            const char* colour_string = settings.GetString("colour_type");
            StringBuffer32 string(colour_string);
            StringBuffer32 lower_string = string.to_lower();

            ECS::Colour::Type colour_type = ECS::Colour::s_stringToType.at(lower_string);
            settings.AddInt("colour_type", (int)colour_type);
        }
    }
}

void PopulateMetaData(const char* id, const RectF& rect, ECS::EntityMetaData& meta_data)
{
    meta_data.data.AddString("id", id);
    meta_data.data.AddVectorF("position", rect.TopLeft());
    meta_data.data.AddVectorF("size", rect.Size());
    meta_data.data.AddString("sprite", id);
}
