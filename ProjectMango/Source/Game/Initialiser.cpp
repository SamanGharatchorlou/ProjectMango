#include "pch.h"
#include "Initialiser.h"

static std::vector<ComponentInitialiser*> s_compInitialisers;

ComponentInitialiser::ComponentInitialiser()
{ 
	s_compInitialisers.push_back(this); 
}

void ComponentInitialiser::InitAll()
{
	bool defined_components[ECS::Component::Count] = { false };

	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->OnInit();

		defined_components[s_compInitialisers[i]->GetType()] = true;
	}

	for (u32 i = 0; i < ECS::Component::Count; i++)
	{
		if (!defined_components[i])
		{
			DebugPrint(Warning, "%s component has not been defined, add this to ComponentsSetup.cpp", ECS::ComponentNames[i]);
		}
	}
}

void ComponentInitialiser::RemoveAll(ECS::Entity entity)
{
	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->Remove(entity);
	}
}