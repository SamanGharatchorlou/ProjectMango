#include "pch.h"
#include "Camera.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Biome.h"
#include "Characters/Player/PlayerCharacter.h"

Camera* Camera::Get()
{
	static Camera sInstance;
	return &sInstance;
}

void Camera::clear()
{
	shakeyCam.clear();
}

Camera::Camera() : mScale(1.0f) { }

void Camera::setScale(float scale)
{
	mScale = scale;
	setViewport(rect.Size() / mScale);
}

void Camera::setViewport(VectorF viewport) 
{ 
	rect.SetSize(viewport); 
}

void Camera::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Transform* transform = ecs->GetComponent(Transform, targetEntity);
	if(!transform)
		return;

	ECS::Entity player = Player::Get();
	const ECS::Level& level = ECS::Biome::GetLevel(player);
	
	const VectorF translation = (transform->GetObjectCenter() - rect.Center()) * dt * 5.0f;
	rect.Translate( translation );

	VectorF bounds_translation;

	// restrict within x bounds
	const float left_point = level.worldPos.x;
	const float right_point = level.worldPos.x + level.size.x;
	if(rect.LeftPoint() < left_point)
	{
		bounds_translation.x = left_point - rect.LeftPoint();
	}
	else if(rect.RightPoint() > right_point)
	{
		bounds_translation.x = right_point - rect.RightPoint();
	}

	// restrict within y bounds
	const float top_point = level.worldPos.y;
	const float bot_point = level.worldPos.y + level.size.y;
	if (rect.TopPoint() < top_point)
	{
		bounds_translation.y = top_point - rect.TopPoint();
	}
	else if (rect.BotPoint() > bot_point)
	{
		bounds_translation.y = bot_point - rect.BotPoint();
	}
	
	rect.Translate(bounds_translation);

	if (shakeyCam.hasTrauma())
	{
		//mActiveRect = shakeyCam.rect();

#if PRINT_SHAKEYCAM_VALUES
		DebugPrint(Log, "Trauma %.3f | Offset = %.3f, %.3f",
			shakeyCam.trauma(),
			shakeyCam.offset().x, shakeyCam.offset().y);
#endif
	}
	else
	{
		//mActiveRect = &mRect;
	}
}
