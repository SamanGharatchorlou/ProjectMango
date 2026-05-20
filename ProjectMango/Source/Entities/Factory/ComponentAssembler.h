#pragma once

namespace ECS { struct EntityMetaData; }

//bool AddTransform(ECS::Entity entity, const ECS::EntityMetaData& emd);
//bool TryAddSprite(ECS::Entity entity, const ECS::EntityMetaData& emd);
//bool TryAddAnimator(ECS::Entity entity, const ECS::EntityMetaData& emd);
//bool TryAddUIButton(ECS::Entity entity, const ECS::EntityMetaData& emd);
//bool TryAddCallback(ECS::Entity entity, const ECS::EntityMetaData& emd);
//bool TryAddFaction(ECS::Entity entity, const ECS::EntityMetaData& emd);

ECS::Entity AssembleEntity(const ECS::EntityMetaData& emd);