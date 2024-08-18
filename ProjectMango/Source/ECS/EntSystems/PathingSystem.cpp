#include "pch.h"
#include "PathingSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/TileMap.h"
#include "ECS/Components/ComponentCommon.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "Characters/States/EnemyStates.h"

namespace ECS
{
	//static int heuristic(VectorI from, VectorI to)
	//{
	//	return (from - to).lengthSquared();
	//}
	//
	//using TileCost = std::pair<VectorI, int>;

	//struct GreaterThanByCost
	//{
	//	bool operator ()(TileCost left, TileCost right)
	//	{
	//		return left.second > right.second;
	//	};
	//};

	//static std::vector<VectorI> getPath(VectorI start, VectorI finish, std::vector< std::pair<VectorI, VectorI>>& visited)
	//{
	//	std::vector<VectorI> path;
	//	path.push_back(finish);

	//	while(path.back() != start)
	//	{
	//		for( u32 i = 0; i < visited.size(); i++ )
	//		{
	//			if(visited[i].second == path.back())
	//			{
	//				path.push_back(visited[i].first);
	//				break;
	//			}
	//		}
	//	}

	//	return path;
	//}

	//std::vector<VectorI> PathingSystem::FindPath(VectorF startPosition, VectorF endPosition)
	//{
	//	const TileMap* map = TileMap::GetActive();

	//	const VectorI tile_count = (map->tileMap.mapSize / c_tileSize).toInt();
	//	const VectorF world_tile_size = TileMap::GetMapSizeRatio() * c_tileSize;

	//	const VectorI start_index = (startPosition / world_tile_size).toInt();
	//	const VectorI end_index = (endPosition / world_tile_size).toInt();

	//	// Lowest to highest path cost queue
	//	std::priority_queue<TileCost, std::vector<TileCost>, GreaterThanByCost> frontier;
	//	frontier.push(TileCost(start_index, 0));

	//	std::vector< std::pair<VectorI, VectorI>> visited;

	//#if DEBUG_MODE
	//	int loopCounter = 0;
	//#endif

	//	int pathCount = 0;

	//	while (!frontier.empty())
	//	{
	//		const VectorI index = frontier.top().first;
	//		frontier.pop();
	//		const VectorI neighbours[4] = {
	//									index + VectorI(0,-1), index + VectorI( 1, 0),
	//									index + VectorI(0, 1), index + VectorI(-1, 0) };

	//		// Search all neighbours
	//		for (unsigned int i = 0; i < 4; i++)
	//		{
	//			const VectorI next_index = *(neighbours + i);

	//			bool valid = next_index.isPositive() && next_index.x < tile_count.x && next_index.y < tile_count.y;

	//			if (valid)
	//			{
	//				bool has_visited = false;
	//				for( u32 i = 0; i < visited.size(); i++ )
	//				{
	//					if(visited[i].second == next_index)
	//					{
	//						has_visited = true;
	//					}
	//				}

	//				if (!has_visited)
	//				{
	//					int priority = heuristic(end_index, next_index);
	//					frontier.push(TileCost(next_index, priority));

	//					visited.push_back( std::pair(index,next_index));

	//					if (next_index == end_index)
	//					{
	//						return getPath(start_index, end_index, visited);
	//					}
	//				}
	//			}
	//		}

	//#if DEBUG_MODE
	//		if (frontier.size() > tile_count.x * tile_count.y)
	//		{
	//			return std::vector<VectorI>();
	//		}

	//		loopCounter++;
	//#endif
	//	}

	//#if DEBUG_MODE
	//	int largeNumberOfLoops = 50;
	//	if (loopCounter > largeNumberOfLoops)
	//	{
	//		DebugPrint(Log,
	//			"Empty frontier: No valid path was found %d loops were made until this failed\n", loopCounter);
	//	}
	//#endif
	//	
	//	return std::vector<VectorI>();
	//}

	//static VectorF GetBestAttackPosition(const Pathing& pathing)
	//{
	//	EntityCoordinator* ecs = GameData::Get().ecs;
	//	
	//	VectorF best_target_pos(-1, -1);
	//	Direction best_direction = Direction::Count;

	//	if(ecs->IsAlive(pathing.target))
	//	{
	//		const RectF target_rect = ECS::GetColliderRect(pathing.target);
	//		const VectorF start_position = ECS::GetPosition(pathing.entity);

	//		int min_path_length = INT_MAX;

	//		// check which position is most desirable out of top, right, bottom, left depending on our position
	//		for( u32 i = 0; i < Direction::Count; i++ )
	//		{
	//			const VectorI direction = s_directions[i];
	//			const VectorI facing_direction = direction *-1;
	//			
	//			VectorF pos = target_rect.Center();
	//			const VectorF area = Enemy::GetAttackRangeArea(pathing.entity, facing_direction) * 0.6f;
	//			pos += area * direction.toFloat();

	//			if(DebugMenu::GetSelectedEntity() == pathing.entity)
	//				DebugMenu::SetPathAttackPoint(pos, Colour::Purple, (Direction)i);

	//			u32 path_length = (u32)PathingSystem::FindPath(start_position, pos).size();
	//			if(path_length < min_path_length)
	//			{
	//				min_path_length = path_length;
	//				best_target_pos = pos;
	//				best_direction = (Direction)i;
	//			}
	//		}
	//	}

	//	if(DebugMenu::GetSelectedEntity() == pathing.entity && best_direction != Direction::Count)
	//		DebugMenu::SetPathAttackPoint(best_target_pos, Colour::Green, best_direction);

	//	return best_target_pos;
	//}

	void PathingSystem::Update(float dt)
	{
		//EntityCoordinator* ecs = GameData::Get().ecs;
		//
		//const TileMap* tm = TileMap::GetActive();
		//
		//if(!tm)
		//	return;

		//const VectorF world_tile_size = TileMap::GetMapSizeRatio() * c_tileSize;
		//const VectorI map_dim = (tm->tileMap.mapSize / c_tileSize).toInt();

 	//	for (Entity entity : entities)
		//{
		//	Pathing& pathing = ecs->GetComponentRef(Pathing, entity);

		//	if(DebugMenu::RemovePath(entity))
		//	{
		//		pathing.path.clear();
		//		continue;
		//	}

		//	const VectorF start_position = ECS::GetPosition(entity);
		//	const VectorF end_position = GetBestAttackPosition(pathing);
		//	
		//	// only update if the target has moved
		//	const VectorI end_index = (end_position / world_tile_size).toInt();
		//	const VectorI start_index = (start_position / world_tile_size).toInt();
		//	const bool valid_end = end_index.isPositive() && end_index.x < map_dim.x && end_index.y < map_dim.y;
		//	const bool valid_start = start_index.isPositive() && start_index.x < map_dim.x && start_index.y < map_dim.y;

		//	if(!valid_end || !valid_start)
		//	{
		//		pathing.path.clear();
		//		continue;
		//	}

		//	const bool update_path = start_index != pathing.currentStart || end_index != pathing.currentTarget;
		//	if(update_path)
		//	{
		//		pathing.currentTarget = end_index;	
		//		pathing.currentStart = start_index;
		//		if(valid_start && valid_end)
		//		{
		//			pathing.path = FindPath(start_position, end_position);
		//		}
		//	}
		//}
	}
}