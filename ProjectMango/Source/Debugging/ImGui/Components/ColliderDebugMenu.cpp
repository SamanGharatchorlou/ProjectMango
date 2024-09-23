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
			ImGui::Text("IsFloor");
		if (collider.HasFlag(ECS::Collider::IsWall))
			ImGui::Text("IsWall");
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

		// left/right collisions
		bool collide_top = collider.collisionSide[ECS::Collider::Top];
		bool collide_right = collider.collisionSide[ECS::Collider::Right];
		bool collide_bot = collider.collisionSide[ECS::Collider::Bottom];
		bool collide_left = collider.collisionSide[ECS::Collider::Left];
			
		ImGui::Text("Collision Sides: %d, %d, %d, %d", collide_top, collide_right, collide_bot, collide_left);

		ImGui::Text("Allowed Movement: %f, %f", collider.allowedMovement.x, collider.allowedMovement.y);
		ImGui::Text("Desired Movement: %f, %f", collider.desiredMovement.x, collider.desiredMovement.y);

		ImGui::PopID();
	}

	return type;
}