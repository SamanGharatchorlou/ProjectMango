#pragma once

namespace ECS
{
	using Entity = u32;		// simple id for each entity
}

struct ComponentInitialiser
{
	ComponentInitialiser();

	static void InitAll();
	static void RemoveAll(ECS::Entity entity);

	// need to remove?
	//~Initialiser() { s_initialisers.ear(this); }

	virtual void OnInit() = 0;
	virtual void OnExit() { };

	virtual void Remove(ECS::Entity entity) = 0;
};
