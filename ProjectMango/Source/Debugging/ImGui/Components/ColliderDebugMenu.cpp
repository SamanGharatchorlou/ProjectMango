#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

ECS::Component::Type DebugMenu::DoColliderDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Collider;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Collider& collider = ecs->GetComponentRef(Collider, entity);
		ImGui::PushID(entity + (int)type);

		
        if (collider.HasFlag(ECS::Collider::Static))
			ImGui::Text("Static");
        if (collider.HasFlag(ECS::Collider::IsPlayer))
			ImGui::Text("IsPlayer");
        if (collider.HasFlag(ECS::Collider::IsEnemy))
			ImGui::Text("IsEnemy");
		if (collider.HasFlag(ECS::Collider::IsTerrain))
			ImGui::Text("IsTerrain");
		if (collider.HasFlag(ECS::Collider::IsFloor))
			ImGui::Text("IsTerrain");
		if (collider.HasFlag(ECS::Collider::IsWall))
			ImGui::Text("IsTerrain");
		if (collider.HasFlag(ECS::Collider::IsDamage))
			ImGui::Text("IsDamage");
		if (collider.HasFlag(ECS::Collider::IgnoreAll))
			ImGui::Text("IgnoreAll");
		if (collider.HasFlag(ECS::Collider::IgnoreDamage))
			ImGui::Text("IgnoreDamage");
		if (collider.HasFlag(ECS::Collider::TerrainOnly))
			ImGui::Text("TerrainOnly");
		if (collider.HasFlag(ECS::Collider::GhostCollider))
			ImGui::Text("GhostCollider");
		if (collider.HasFlag(ECS::Collider::CanBump))
			ImGui::Text("CanBump");
			
		ImGui::PopID();
	}

	return type;
}