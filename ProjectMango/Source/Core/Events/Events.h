#pragma once

//class Enemy;
class TargePositionAttackAbility;
class Texture;


enum class Event
{
	None,

	EnemyDead,
	PlayerDead,

	HealthChanged,

	SetGameVolume,
	SetMusicVolume,

	UpdateLoadingBar,
	UpdateTextBox,
	SetTextColour,
	SetUIBar,
	SetUISlider,
	OpenPopup,

	Trauma,

	LevelUpdated,
	MapLevel,

	ActivateAbilityOn,

	Render,

#if UI_EDITOR
	MoveUIElement,
	ChangeUIElementSize,
#endif
};


struct EventData
{
	EventData() { }
	EventData(Event type) : eventType(type) {}
	virtual ~EventData() { }

	void setEventType(Event event) { eventType = event; }
	Event eventType = Event::None;
};


struct EventPacket
{
	EventPacket(EventData* dataPtr) : data(dataPtr) { }
	~EventPacket() { }

	void free()
	{
		delete data;
		data = nullptr;
	}

	EventData* data;
};



struct MapLevelEvent : public EventData
{
	MapLevelEvent(int mapLevel) : mMapLevel(mapLevel), EventData(Event::MapLevel) { }
	int mMapLevel;
};


struct PlayerDeadEvent : public EventData
{
	PlayerDeadEvent() : mKills(-1), mScore(-1), mMapLevel(-1), EventData(Event::PlayerDead) { }
	PlayerDeadEvent(int kills, int score, int mapLevel) : mKills(kills), mScore(score), mMapLevel(mapLevel), EventData(Event::PlayerDead) { }
	int mKills;
	int mScore;
	int mMapLevel;
};


struct OpenPopupEvent : public EventData
{
	OpenPopupEvent(const char* infoID) : EventData(Event::OpenPopup), mInfoID(infoID) { }

	const StringBuffer32 mInfoID;
};


struct LevelUpdatedEvent : public EventData
{
	enum MapStatus
	{
		Added,
		Removed
	};

	LevelUpdatedEvent(MapStatus status) : EventData(Event::LevelUpdated), mStatus(status) { }
	MapStatus mStatus;
};

struct RenderEvent : public EventData
{
	RenderEvent(Texture* texture, RectF rect, int renderLayer) : EventData(Event::Render), mTexture(texture), mRect(rect), mRenderLayer(renderLayer) { }

	Texture* mTexture;
	RectF mRect;
	int mRenderLayer;
};


struct SetTextColourEvent : public EventData
{
	SetTextColourEvent(const char* id, SDL_Color colour) : EventData(Event::SetTextColour), mId(id), mColour(colour) { }

	SDL_Color mColour;
	const StringBuffer32 mId;
};


struct HealthChangedEvent : public EventData
{
	HealthChangedEvent() : EventData(Event::HealthChanged) { }
};



struct SetUISlider : public EventData
{
	SetUISlider(const char* id, float value) : EventData(Event::SetUISlider), mId(id), mValue(value) { }

	const StringBuffer32 mId;
	float mValue;
};


struct ActivateAreaAttack : public EventData
{
	ActivateAreaAttack(const TargePositionAttackAbility* ability) : EventData(Event::ActivateAbilityOn), mAbility(ability) { }

	const TargePositionAttackAbility* mAbility;
};


struct UpdateLoadingBarEvent : public EventData
{
	UpdateLoadingBarEvent() : EventData(Event::UpdateLoadingBar) { }
};


//struct EnemyDeadEvent : public EventData
//{
//	EnemyDeadEvent(const Enemy* enemy, int score, int exp) : EventData(Event::EnemyDead), mEnemy(enemy), mScore(score), mExp(exp) { }
//
//	const Enemy* mEnemy;
//	const int mScore;
//	const int mExp;
//};


struct SetUIBarEvent : public EventData
{
	SetUIBarEvent(const char* bar, const char* barContainer, const float percentage) : EventData(Event::SetUIBar),
		mBar(bar), mBarContainer(barContainer), mPercentage(percentage) { }

	const StringBuffer32 mBar;
	const StringBuffer32 mBarContainer;
	const float mPercentage;
};


struct UpdateTextBoxEvent : public EventData
{
	UpdateTextBoxEvent(const char* id, const char* text) : EventData(Event::UpdateTextBox), mId(id), mText(text) { }

	const StringBuffer32 mId;
	const BasicString mText;
};


struct TraumaEvent : public EventData
{
	TraumaEvent(float trauma) : EventData(Event::Trauma), mTrauma(trauma) { }

	const float mTrauma;
};


struct SetVolumeEvent : public EventData
{
	SetVolumeEvent(float volume) : mVolume(volume) { } 

	const float mVolume;
};



#if UI_EDITOR
struct EditUIRectEvent : public EventData
{
	EditUIRectEvent(const BasicString& id, VectorF change) : mId(id), mChange(change) { };
	~EditUIRectEvent() { };

	const BasicString mId;
	const VectorF mChange;
};
#endif