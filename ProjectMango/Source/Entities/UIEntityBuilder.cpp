#include "UIEntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"
#include "EntityBuilder.h"

#include "Entities/CardRegistry.h"

using namespace ECS;

typedef Entity(*CreateEntityFn)(const EntityMetaData&);

Entity CreateUICursor()
{
	Entity entity = CreateEntity("Cursor", true);

	const Config* config = GetConfigFromEntity(entity);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(config, VectorF());
	transform.renderOffset = transform.size * -0.5;

	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.renderLayer = RenderLayer::Top;
	sprite.canFlip = false;
	sprite.Init(config);

	UICursor& cursor = AddComponent(UICursor, entity);
	InputManager* input = InputManager::Get();
	cursor.cursor = &input->mCursor;

	return entity;
}


static void GenerateCardCostChildren(ECS::Entity entity)
{
	if(Card* card = GetComponent(Card, entity))
	{	
		const Transform& transform = GetComponentRef(Transform, entity);
		const Sprite& sprite = GetComponentRef(Sprite, entity);

		int count = 0;
		for( u32 i = 0; i < Coin::Count; i++ )
		{
			if(card->cost[i] > 0)
			{
				Entity child_entity = CreateBasicObject("card_icon", VectorF(64,64));
				EntityData::SetParent(child_entity, entity);

				// where on the card is it placed
				int use_count = 0;
				if(count > 0)
					use_count = 1;
				if( count > 2)
					use_count *= 2;
				if(count % 2)
					use_count = -use_count;
			
				Transform& child_transform = GetComponentRef(Transform, child_entity);
				VectorF visible_size = child_transform.size * 0.8f;
				VectorF position = (transform.size - child_transform.size) * 0.5f;
				position += VectorF(0, child_transform.size.y * 0.8f) * (float)use_count;
				child_transform.SetLocalPosition( position );
			
				const StringBuffer32& colour_string = Coin::s_typeToString.at((Coin::Type)i);

				char buffer[32];
				snprintf(buffer, 32, "%s_gem", colour_string.c_str());

				Sprite& child_sprite = GetComponentRef(Sprite, child_entity);
				child_sprite.SetTexture(buffer);
				child_sprite.renderLayer = (RenderLayer)((int)sprite.renderLayer + 1);

				UIText& child_text_display = AddComponent(UIText, child_entity);

				BasicString text(card->cost[i]);
				child_text_display.SetText( text.c_str() );
				child_text_display.FitToSize(visible_size);
				child_text_display.SetRenderOffsetToCenter();

				count++;
			}
		}
	}
}

Entity CreateCardEntity(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	Card& card = AddComponent(Card, entity);
	CardRegistry::GetRandomTier1Card(card);

	GenerateCardCostChildren(entity);

	return entity;
}

Entity RecreateCardFromCard(ECS::Entity previous_Card)
{
	ECS::Entity entity = ECS::CreateEntity( "card" );

	// copy over transform and sprite data
	ECS::Transform& transform = AddComponent(Transform, entity);
	CopyComponent(transform, GetComponentRef(Transform, previous_Card));

	ECS::Sprite& sprite = AddComponent(Sprite, entity);
	CopyComponent(sprite, GetComponentRef(Sprite, previous_Card));

	AddComponent(UIButton, entity);

	// draw a random card
	ECS::Card& new_card = AddComponent(Card, entity);
	CardRegistry::GetRandomTier1Card( new_card );

	GenerateCardCostChildren(entity);

	return entity;
}

//static ECS::Entity CreateButton(const ECS::EntityMetaData& emd)
//{
//	return CreateBasicObject(emd);
//}
//
//Entity CreateText(const EntityMetaData& emd)
//{
//	return CreateBasicObject(emd);
//}

void CreateUIEntities()
{
	CreateUICursor();
}

//static std::unordered_map<BasicString, CreateEntityFn> s_createEntitiyFunctions;
//
//void CreateUIEntities()
//{
//	s_createEntitiyFunctions.clear();
//
//	// UI entities
//	s_createEntitiyFunctions["Cursor"] = CreateUICursor;
//
//	// these dont seem correct, its not often ill just want to create an entity called button that is a button
//	// is first some other entity, e.g. a card, and i add a button thing to it, to make it also be a button
//	s_createEntitiyFunctions["Text"] = CreateText;
//	s_createEntitiyFunctions["Button"] = CreateButton;
//}

//// accessors
//bool IsUIEntity(const char* entity_type)
//{
//	return s_createEntitiyFunctions.contains(entity_type);
//}
//ECS::Entity CreateUIEntity(const char* entity_type, const ECS::EntityMetaData& emd)
//{
//	return s_createEntitiyFunctions.at(entity_type)(emd);
//}