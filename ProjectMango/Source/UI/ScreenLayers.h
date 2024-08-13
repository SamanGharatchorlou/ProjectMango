#pragma once

class UIElement;
class Screen;
enum class UIType;

struct LayerItem
{
	StringMap32 attributes;
	BasicString value;
	UIType type;
};

typedef std::vector<UIElement*> Elements;
typedef std::vector<LayerItem> LayerItems;
typedef std::vector<LayerItems> ScreenItems;

struct ScreenLayer
{
	ScreenLayer(const char* name) : id(name) { }
	void addElements(Elements newElements) { elements.insert(elements.end(), newElements.begin(), newElements.end()); }

	const StringBuffer32 id;
	Elements elements;
};

struct ScreenLayers
{
	Elements elementList() const;
	//ScreenLayer* layer(const char* id) const;
	UIElement* find(const char* id) const;
	UIElement* find(const char* id, UIType type) const;

	std::vector<ScreenLayer> mLayers;
};

namespace ScreenBuilder
{
	void populateScreen(Screen& screen);
	void readScreen(const char* screen, ScreenItems& screenItems);
};