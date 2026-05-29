#include "pch.h"
#include "Helpers.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"
#include "Graphics/Raycast.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "System/Window.h"


float EaseOut(float value, int easing_factor)
{
	return 1.0f - Power<float>(1.0f - value, easing_factor);
}


bool IsPlayer(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsPlayer);

	return false;
}
bool IsEnemy(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsEnemy);

	return false;
}
bool IsTerrain(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsTerrain);

	return false;
}
//
//bool ContainsPoint(const RectF& rect, VectorF point)
//{
//	return !(	point.x > rect.RightPoint() || 
//			point.x < rect.LeftPoint()  || 
//			point.y > rect.BotPoint()   || 
//			point.y < rect.TopPoint());
//}



bool IsTargetInFrontOfSource(ECS::Entity target, ECS::Entity source)
{
	VectorF source_pos = ECS::GetPosition(source);
	VectorI facing = ECS::GetFacingDirectionVector(source);

	VectorF target_pos = ECS::GetPosition(target);
	// target pos relative to source
	target_pos = target_pos - source_pos;

	if(facing.x > 0)
	{
		return target_pos.x > 0;
	}
	else
	{
		return target_pos.x < 0;
	}
}

bool IsSelectedDebugEntity(ECS::Entity entity)
{
	return DebugMenu::GetSelectedEntity() == entity;
}

float RevertFromScreenSize(float size)
{
	const Window* window = GameData::Get().window;
	return size * window->fakeSize().area() / window->realSize().area();
}

float AdjustToScreenSize(float size)
{
	const Window* window = GameData::Get().window;
	return size * window->realSize().area() / window->fakeSize().area();
}

VectorF AdjustToScreenSize(VectorF size)
{
	const Window* window = GameData::Get().window;
	return size * window->realSize() / window->fakeSize();
}