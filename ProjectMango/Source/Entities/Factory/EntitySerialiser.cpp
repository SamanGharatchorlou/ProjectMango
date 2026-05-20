#include "pch.h"
#include "EntitySerialiser.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

//#include "ECS/Components/ComponentHelpers.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace rapidjson;

void ReadMetaDataFromJson(rapidjson::Value& jsonValue, LevelSizeInfo sizeInfo, ECS::EntityMetaData& metaData )
{
    Settings& settings = metaData.data;

    settings.strings["Id"] = jsonValue["__identifier"].GetString();

    float width = jsonValue["width"].GetFloat();
    float height = jsonValue["height"].GetFloat();

    VectorF level_to_window;
    level_to_window.x = 1.0f / sizeInfo.windowToLevel.x;
    level_to_window.y = 1.0f / sizeInfo.windowToLevel.y;

    Value& px = jsonValue["px"];
    float px_x = px[0].GetFloat(); // + (width * 0.5f);
    float px_y = px[1].GetFloat(); // - (height);
    settings.vectors["Position"] = (VectorF(px_x, px_y) * level_to_window) + sizeInfo.LevelWorldPos;
    settings.vectors["Size"] = (VectorF(width, height) * level_to_window);

    Value& pivot = jsonValue["__pivot"];
    float pivot_x = pivot[0].GetFloat();
    float pivot_y = pivot[1].GetFloat();
    settings.vectors["PivotPoint"] = VectorF(pivot_x, pivot_y);

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
        if (settings.Contains("SizeOverride"))
        {
            VectorF size = settings.GetVector("SizeOverride");
            settings.vectors["Size"] = (size * level_to_window);
        }
        else if (settings.Contains("ColourType"))
        {
            const char* colour_string = settings.GetString("ColourType");
            StringBuffer32 string(colour_string);
            StringBuffer32 lower_string = string.to_lower();

            ECS::Colour::Type colour_type = ECS::Colour::s_stringToType.at(lower_string);
            settings.values["ColourType"] = (float)colour_type;
        }
    }
}

void ReadMetaDataFromEntity(ECS::Entity entity, ECS::EntityMetaData& out_data)
{
    const ECS::Transform& transform = GetComponentRef(Transform, entity);
    out_data.data.vectors["Position"] = transform.worldPosition;
    out_data.data.vectors["Size"] = transform.size;

    if (const ECS::Sprite* sprite = GetComponent(Sprite, entity))
    {
        out_data.data.strings["Sprite"] = sprite->image.id;
    }

    if (const ECS::UIText* text = GetComponent(UIText, entity))
    {
        out_data.data.values["PtSize"] = (float)text->font.GetPtSize();
        out_data.data.strings["Text"] = text->text.c_str();
    }
}

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
static Value SerializeEntityToLDtk(Document::AllocatorType& alloc,
    const LevelSizeInfo& sizeInfo, const ECS::EntityMetaData& meta_data)
{
    Value entity(kObjectType);
    const Settings& md = meta_data.data;

    // --- Identity ---
    const char* id = md.Contains("Id") ? md.GetString("Id") : "Unknown";
    entity.AddMember("__identifier", Value(id, alloc), alloc);
    entity.AddMember("iid", Value("", alloc), alloc); // generate a UUID if needed

    // --- Size (inverse of level_to_window scale) ---
    VectorF size = md.GetVector("Size") * sizeInfo.windowToLevel;
    entity.AddMember("width", size.x, alloc);
    entity.AddMember("height", size.y, alloc);

    // --- Position (px): undo the offset + scale from ReadMetaData ---
    VectorF world_pos = md.GetVector("Position");
    VectorF level_pos = (world_pos - sizeInfo.LevelWorldPos) * sizeInfo.windowToLevel;
    Value px(kArrayType);
    px.PushBack(level_pos.x, alloc);
    px.PushBack(level_pos.y, alloc);
    entity.AddMember("px", px, alloc);

    // --- Pivot ---
    VectorF pivot = md.Contains("PivotPoint") ? md.GetVector("PivotPoint") : VectorF(0.5f, 0.5f);
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
        if (key == "Position" || key == "Size" || key == "PivotPoint")
            continue;

        Value arr(kArrayType);
        arr.PushBack(val.x, alloc);
        arr.PushBack(val.y, alloc);
        WriteFieldInstance(alloc, fields, key.c_str(), std::move(arr), "Array<Float>");
    }

    entity.AddMember("fieldInstances", fields, alloc);
    return entity;
}

void SaveEntityToJson(const char* filepath,
    const LevelSizeInfo& sizeInfo, const std::vector<ECS::EntityMetaData>& meta_data)
{
    FILE* f = fopen(filepath, "w");
    if (!f)
        return;

    Document doc;
    doc.SetArray();
    auto& alloc = doc.GetAllocator();

    for (u32 i = 0; i < meta_data.size(); i++)
    {
        rapidjson::Value entityJson = SerializeEntityToLDtk(alloc, sizeInfo, meta_data[i]);
        doc.PushBack(entityJson, alloc);
    }

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    fwrite(buffer.GetString(), 1, buffer.GetSize(), f);

    fclose(f);
}

bool LoadEntityFromJson(const char* filepath,
    const LevelSizeInfo& sizeInfo, std::vector<ECS::EntityMetaData>& metaData)
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
            ReadMetaDataFromJson(json_value, sizeInfo, meta_data);
        }
    }

    // doc IS the entity object — pass directly to your existing reader
    return true;
}