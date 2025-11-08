#pragma once

namespace ECS{ struct Firearm; }

ECS::Entity CreateBasicBullet(ECS::Firearm& Firearm);
ECS::Entity EquipFirearm(ECS::Entity handle, const char* firearm_id);