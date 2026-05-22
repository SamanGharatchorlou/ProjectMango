#include "pch.h"
#include "ComponentAssembler.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "System/Window.h"

using namespace ECS;
using namespace rapidjson;

void PopulateMetaData(Value& jsonValue, EntityMetaData& metaData)
{
	Settings& settings = metaData.data;

	settings.strings["id"] = jsonValue["__identifier"].GetString();

	float width = jsonValue["width"].GetFloat();
	float height = jsonValue["height"].GetFloat();

	VectorF window_to_level = GameData::Get().window->windowToLevel;

	VectorF level_to_window;
	level_to_window.x = 1.0f / window_to_level.x;
	level_to_window.y = 1.0f / window_to_level.y;

	Value& px = jsonValue["px"];
	float px_x = px[0].GetFloat(); // + (width * 0.5f);
	float px_y = px[1].GetFloat(); // - (height);
	settings.vectors["position"] = (VectorF(px_x, px_y) * level_to_window);
	settings.vectors["size"] = (VectorF(width, height) * level_to_window);

	Value& pivot = jsonValue["__pivot"];
	float pivot_x = pivot[0].GetFloat();
	float pivot_y = pivot[1].GetFloat();
	settings.vectors["pivot_point"] = VectorF(pivot_x, pivot_y);

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
			settings.vectors["size"] = (size * level_to_window);
		}
		else if (settings.Contains("colour_type"))
		{
			const char* colour_string = settings.GetString("colour_type");
			StringBuffer32 string(colour_string);
			StringBuffer32 lower_string = string.to_lower();

			ECS::Colour::Type colour_type = ECS::Colour::s_stringToType.at(lower_string);
			settings.values["colour_type"] = (float)colour_type;
		}
	}
}

void PopulateMetaData(ECS::Entity entity, ECS::EntityMetaData& out_data)
{
	const ECS::Transform& transform = GetComponentRef(Transform, entity);
	out_data.data.vectors["position"] = transform.worldPosition;
	out_data.data.vectors["size"] = transform.size;

	if (const ECS::Sprite* sprite = GetComponent(Sprite, entity))
	{
		out_data.data.strings["sprite"] = sprite->image.id;
	}

	if (const ECS::UIText* text = GetComponent(UIText, entity))
	{
		out_data.data.values["pt_size"] = (float)text->font.GetPtSize();
		out_data.data.strings["text"] = text->text.c_str();
	}
}

void PopulateMetaData(const char* id, const RectF& rect, ECS::EntityMetaData& meta_data)
{
	meta_data.data.strings["id"] = id;
	meta_data.data.vectors["position"] = rect.TopLeft();
	meta_data.data.vectors["size"] = rect.Size();
	meta_data.data.strings["sprite"] = id;
}

template<class T>
static bool TryAddComponent(Entity entity, const EntityMetaData& emd)
{
	bool requires_component = emd.data.Contains(T::kRequirement);
	if (!requires_component)
		return false;

	T& component = ecs->AddComponent<T>(entity);
	component.Init(emd);
	return true;
}

static bool AddTransform(Entity entity, const ECS::EntityMetaData& emd)
{
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);

	return true;
}

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateEntity(emd);

	AddTransform(entity, emd);

	TryAddComponent<Physics>(entity, emd);
	TryAddComponent<Collider>(entity, emd);

	TryAddComponent<Sprite>(entity, emd);
	TryAddComponent<SpriteSheet>(entity, emd);
	TryAddComponent<Animator>(entity, emd);

	TryAddComponent<UIButton>(entity, emd);
	TryAddComponent<UIText>(entity, emd);

	TryAddComponent<Health>(entity, emd);
	TryAddComponent<Callback>(entity, emd);
	TryAddComponent<Faction>(entity, emd);
	TryAddComponent<Card>(entity, emd);

	return entity;
}