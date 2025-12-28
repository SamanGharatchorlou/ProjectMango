#include "pch.h"
#include "AIControllerSystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	float GetAttackRange(Entity entity, Action::Enum attack)
	{
		if(const BehaviourState* b_state = GetComponent(BehaviourState, entity))
		{
			if(b_state->attackData.contains(attack))
			{
				const AttackStateData& asd = b_state->attackData.at(attack);

				const Transform& transform = GetComponentRef(Transform, entity);
				const VectorF pos =  transform.worldPosition + transform.size * asd.hitBoxPos;
				const VectorF size = transform.size * asd.hitBoxSize;
				const RectF collider_rect(pos, size);

				const VectorF position = transform.GetObjectCenter();
				const float distance = Maths::Max( std::abs(position.x - collider_rect.RightCenter().x), std::abs(position.x - collider_rect.LeftCenter().x) );
				return distance;
			}
		}

		return -1.0f;
	}

	void AIControllerSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			// debug break point
			if(IsSelectedDebugEntity(entity))
				int a = 4;

			AIController& aic = GetComponentRef(AIController, entity);

			if(AIIntent* intent = GetComponent(AIIntent, entity))
			{
				// reset intent
				*intent = AIIntent();

				// grab the target if it has one
				Entity target = Target::GetTarget(entity);

				if(target != EntityInvalid)
				{
					intent->wantsToFaceTarget = true;

					const RectF target_rect = GetRect(target);
					const VectorF position = GetPosition(entity);

					// distance to the target is from the center of the entity 
					// to whichever side of the target is closer
					float distance_a = target_rect.LeftPoint() - position.x;
					float distance_b = target_rect.RightPoint() - position.x;
					float target_distance = Maths::Min( std::abs(distance_a), std::abs(distance_b) );

					if( target_distance < (GetAttackRange(entity, Action::BasicAttack) * 0.8f) )
					{
						intent->wantsToAttack = true;
					}
					
					if(!intent->wantsToAttack)
					{
						intent->wantsToMove = true;
					}
				}
			}

			if(TurnState* turn = GetComponent(TurnState, entity))
			{
				if(turn->isActiveTurn)
				{
					turn->canEndTurn = true;
					if(HasComponent(ActionRequest, entity))
						continue;

					int random_action = Maths::randomNumberBetween(0,2);

					if(random_action == 0)
					{
						ActionRequest& action_request = AddComponent(ActionRequest, entity);
						action_request.request = ActionRequest::CollectCoin;

						int random_colur = Maths::randomNumberBetween(0, Colour::Count);
						CoinStack* cs = CoinStack::GetCoinStack((Colour::Type)random_colur);
						action_request.target = cs->entity; 
					}
					else
					{
						std::vector<Card*> sorted_cards_by_tier;

						ComponentArray<Card>& cards =  GetAllComponents(Card);
						for( auto iter = cards.entityToComponent.begin(); iter != cards .entityToComponent.end(); iter++ )
						{ 
							Card& card = cards.GetComponentByIndex(iter->second);
							sorted_cards_by_tier.push_back(&card);
						}

						std::sort(sorted_cards_by_tier.begin(), sorted_cards_by_tier.end(), [](const Card* a, const Card* b) { 
							return a->tier > b->tier;
						});

						for( Card* card : sorted_cards_by_tier )
						{
							if(card->CanAfford(entity))
							{
								ActionRequest& action_request = AddComponent(ActionRequest, entity);
								action_request.request = ActionRequest::AquireCard;
								action_request.target = card->entity;

								break;
							}
						}
					}
				}
			}
		}
	}
}