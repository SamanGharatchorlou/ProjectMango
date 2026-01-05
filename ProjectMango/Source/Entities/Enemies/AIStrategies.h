#pragma once

namespace AIStrategy
{
	enum Difficulty
	{
		Easy,
		Medium,
		Hard
	};

	void BuyCheapestCard(ECS::Entity entity);
	void BuyBestCard(ECS::Entity entity);
	void TakeRandomAction(ECS::Entity entity);

	//enum Phase
	//{
	//	None,
	//	Building,
	//};
}