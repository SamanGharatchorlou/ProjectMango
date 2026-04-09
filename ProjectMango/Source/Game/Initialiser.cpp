#include "pch.h"
#include "Initialiser.h"
#include "ECS/EntityCommon.h"

static std::vector<ComponentInitialiser*> s_compInitialisers;

ComponentInitialiser::ComponentInitialiser()
{ 
	s_compInitialisers.push_back(this); 
}

void ComponentInitialiser::InitAll()
{
	std::vector<int> defined_components;

	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->OnInit();

		defined_components.push_back(s_compInitialisers[i]->GetType());
	}

	if (defined_components.size() != ECS::ComponentCount)
	{
		DebugPrint(Warning, "a component has not been defined initialised properly");
	}
}

void ComponentInitialiser::RemoveAll(ECS::Entity entity)
{
	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->Remove(entity);
	}
}