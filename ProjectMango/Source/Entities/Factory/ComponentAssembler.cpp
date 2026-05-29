#include "pch.h"
#include "ComponentAssembler.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

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

template<class T>
static bool SerialiseComponent(Entity entity, EntityMetaData& out_emd)
{
	const T* component= ecs->GetComponent<T>(entity);
	if (component == nullptr)
		return false;

	component->Serialise(out_emd);
	return true;
}

static bool AddTransform(Entity entity, const ECS::EntityMetaData& emd)
{
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);

	return true;
}

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd, Entity existing_entity)
{
	ECS::Entity entity = existing_entity;
	if(entity == EntityInvalid)
		entity = CreateEntity(emd);

	TryAddComponent<EntityData>(entity, emd);
	TryAddComponent<Transform>(entity, emd);

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
	TryAddComponent<CoinStack>(entity, emd);

	return entity;
}

void SerialiseEntity(ECS::Entity entity, ECS::EntityMetaData& out_data)
{
	SerialiseComponent<EntityData>(entity, out_data);
	SerialiseComponent<Transform>(entity, out_data);
	SerialiseComponent<Sprite>(entity, out_data);
	SerialiseComponent<UIButton>(entity, out_data);
	SerialiseComponent<UIText>(entity, out_data);
	SerialiseComponent<Callback>(entity, out_data);
}

void SetParentsAndChildren(const ECS::EntityMetaData& emd)
{
 	if (const std::vector<u64>* children_uids = emd.data.GetIntArray("children_iids"))
	{
		Entity parent = FindEntityFromIid(emd.data.GetU64("iid"));
		if (parent == EntityInvalid)
			return;

		for (u32 i = 0; i < children_uids->size(); i++)
		{
			u64 uid = children_uids->at(i);
			Entity child = FindEntityFromIid(uid);

			if(child != EntityInvalid)
				EntityData::SetParent(child, parent);
		}
	}
}