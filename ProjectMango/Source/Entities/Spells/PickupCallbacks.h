#pragma once

typedef void(*OnPickupFn)(ECS::Entity pickup_entity, ECS::Entity picker_upper);


void ApplyReboundRune(ECS::Entity rune_entity, ECS::Entity hit_entity);
void ApplyEchoRune(ECS::Entity rune_entity, ECS::Entity hit_entity);
