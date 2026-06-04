#include "pch.h"
#include "ComponentDebugMenu.h"

#include "Core/Helpers.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "ECS/Components/IncludeComponents.h"
#include "imgui-master/imgui.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

#include "Input/InputManager.h"

bool s_displayRect = false;
bool s_displayCharacterPosition = false;
bool s_displayFlipPoint = false;
bool s_outputPosition = false;

u32 DebugMenu::DoTransformDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Transform::TypeName();
	ComponentID type_id = Transform::TypeId();

	ImGui::PushID(entity + (int)type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ImGui::Text("Click-drag to reposition");
		ImGui::Text("Hold-Space and using arrows to resize");

		ECS::Transform& transform = GetComponentRef(Transform, entity);

		// input values
		//float pos[2]{ transform.worldPosition.x, transform.worldPosition.y };
		//if (ImGui::InputFloat2("World Pos", pos))
		//	transform.worldPosition = VectorF(pos[0], pos[1]);

		//float size[2]{ transform.size.x, transform.size.y };
		//if (ImGui::InputFloat2("Size", size))
		//	transform.size = VectorF(size[0], size[1]);

		ImGui::InputVectorF("World Pos", transform.worldPosition);
		ImGui::InputVectorF("Size", transform.size);

		ImGui::Checkbox("Display Object Rect", &s_displayRect);
		if (s_displayRect)
		{
			RectF rect(transform.worldPosition, transform.size);
			DebugDraw::RectOutline(rect, SColour::Blue);
		}

		ImGui::SameLine();
		ImGui::Checkbox("Display Character Position", &s_displayCharacterPosition);
		if (s_displayCharacterPosition)
		{
			VectorF position = transform.GetObjectCenter();
			DebugDraw::Point(position, SColour::Green);
		}

		ImGui::SameLine();
		ImGui::Checkbox("Display Flip Point", &s_displayFlipPoint);
		if (s_displayFlipPoint)
		{
			VectorF flip_point = transform.worldPosition + transform.size / 2.0f;
			if(ECS::Sprite* sprite = GetComponent(Sprite, entity))
			{
				flip_point = transform.worldPosition + transform.GetHorizontalFlipPoint();
			}

			DebugDraw::Point(flip_point, SColour::Red);
		}
	}
	ImGui::PopID();

	return type_id;
}

u32 DebugMenu::DoPhysicsDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Physics::TypeName();
	ComponentID type_id = Physics::TypeId();

	ImGui::PushID(entity + type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Physics& physics = GetComponentRef(Physics, entity);

		ImGui::VectorText("Speed", physics.speed);
		ImGui::Text("Max Speed: %f", physics.maxSpeed);
		ImGui::Text("Acceleration: %f", physics.acceleration);
		ImGui::Text("Is On Floor: %d", physics.onFloor);
	}
	ImGui::PopID();

	return type_id;
}


void DebugMenu::DrawCollider(const ECS::Collider& collider)
{
    SColour colour = SColour::Blue;
    bool is_static = collider.HasFlag(ECS::Collider::Static);
    if (is_static)
    {
        colour = SColour::Purple;
    }

    bool ignore_all = collider.HasFlag(ECS::Collider::IgnoreAll);
    if (ignore_all)
    {
        colour = SColour::LightGrey;
        colour.a = 100;
    }

    if (collider.HasFlag(ECS::Collider::IsEnemy))
    {
        colour = SColour::Red;
        colour.a = 100;
    }
    if (collider.HasFlag(ECS::Collider::IsPlayer))
    {
        colour = SColour::Green;
        colour.a = 100;
    }
    if (collider.HasFlag(ECS::Collider::TerrainOnly))
    {
        colour = SColour::LightGrey;
        colour.a = 100;
    }

    if (collider.HasFlag(ECS::Collider::QuadCollider))
    {
		//float rotation = 0;
		//VectorF about_point;
		//ECS::GetRotationParams(collider.entity, about_point, rotation);

		//QuadF quad = collider.quad;
		//quad.rotate(rotation, about_point);
        DebugDraw::Quad(collider.quad, colour);
    }
	else
	{
		DebugDraw::Shape(DebugRender::DrawType::RectOutline, collider.rect, colour);
	}
}

u32 DebugMenu::DoColliderDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Collider::TypeName();
	ComponentID type_id = Collider::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Collider& collider = GetComponentRef(Collider, entity);
		ImGui::PushID(entity + type_id);

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
		if (collider.HasFlag(ECS::Collider::QuadCollider))
			ImGui::Text("QuadCollider");

		// left/right collisions
		bool collide_top = collider.collisionSide[ECS::Collider::Top];
		bool collide_right = collider.collisionSide[ECS::Collider::Right];
		bool collide_bot = collider.collisionSide[ECS::Collider::Bottom];
		bool collide_left = collider.collisionSide[ECS::Collider::Left];
			
		ImGui::Text("Collision Sides: %d, %d, %d, %d", collide_top, collide_right, collide_bot, collide_left);

		ImGui::Text("Allowed Movement: %f, %f", collider.allowedMovement.x, collider.allowedMovement.y);
		ImGui::Text("Desired Movement: %f, %f", collider.desiredMovement.x, collider.desiredMovement.y);

		if(collider.collisions.size() > 0)
		{
			ImGui::Text("Collides with");
		}
		for( u32 i = 0; i < collider.collisions.size(); i++ )
		{
			const char* name = GetName(collider.collisions[i]);
			ImGui::Text("%d: %s", i, name);
		}

		DrawCollider(collider);

		ImGui::PopID();
	}

	return type_id;
}