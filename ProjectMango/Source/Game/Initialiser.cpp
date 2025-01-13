#include "pch.h"
#include "Initialiser.h"

static std::vector<ComponentInitialiser*> s_compInitialisers;

ComponentInitialiser::ComponentInitialiser() 
{ 
	s_compInitialisers.push_back(this); 
}

void ComponentInitialiser::InitAll()
{
	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->OnInit();
	}
}

void ComponentInitialiser::RemoveAll(ECS::Entity entity)
{
	for( u32 i = 0; i < s_compInitialisers.size(); i++ )
	{
		s_compInitialisers[i]->Remove(entity);
	}
}