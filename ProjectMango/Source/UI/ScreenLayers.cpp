#include "pch.h"
#include "ScreenLayers.h"

#include "Elements/UIElement.h"
#include "Elements/UIBox.h"
#include "Elements/UIButton.h"
#include "Elements/UISwitch.h"
#include "Elements/UITextBox.h"
#include "Elements/UISlider.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"

#include "Screens/Screen.h"
#include "Screens/ScreenHeaders.h"

namespace ScreenBuilder
{
	struct TexturePacket
	{
		Texture* defaultTexture = nullptr;
		Texture* selected = nullptr;
		Texture* hovering = nullptr;
	};

	static void autoSizeRect(UIElement* element)
	{
		RectF rect = element->rect();

		if (rect.Width() < 0)
		{
			UIBox* box = static_cast<UIBox*>(element);

			VectorF textureSize = box->texture()->originalDimentions;
			float ratioXY = textureSize.x / textureSize.y;

			float width = rect.Height() * ratioXY;

			VectorF position(rect.TopLeft());
			VectorF size(width, rect.Height());

			element->setRect(RectF(position, size));
		}
		else if (rect.Height() < 0)
		{
			UIBox* box = static_cast<UIBox*>(element);

			VectorF textureSize = box->texture()->originalDimentions;
			float ratioYX = textureSize.y / textureSize.x;

			float height = rect.Width() * ratioYX;

			VectorF position(rect.TopLeft());
			VectorF size(rect.Width(), height);

			element->setRect(RectF(position, size));
		}
	}

	static Elements setParents(ScreenLayers& layers, const ScreenItems& screenItems)
	{
		Elements parents;

		for (const LayerItems& layerAttributes : screenItems)
		{
			for (const LayerItem& item : layerAttributes)
			{
				if (item.attributes.contains("parent"))
				{
					const char* parentId = item.attributes.at("parent").c_str();
					UIElement* parent = layers.find(parentId);

					if (parent != nullptr)
					{
						const char* elementId = item.attributes.at("id").c_str();
						UIElement* element = layers.find(elementId, item.type);

						element->setParernt(parent);
						parents.push_back(parent);
					}
					else
					{
						DebugPrint(Warning, "No parent was found with label: %s", item.attributes.at("parent").c_str());
					}
				}
			}
		}

		return parents;
	}

	static TexturePacket getButtonTextures(const char* buttonType)
	{
		TexturePacket buttonTextures;
		const TextureManager* textures = TextureManager::Get();

		XMLParser button(FileManager::Get()->findFile(FileManager::Configs, "Buttons").c_str());
		XMLNode buttonNode = button.rootChild(buttonType);

#if DEBUG_MODE
		if (!buttonNode)
		{
			DebugPrint(Warning, "Button config does not have a '%s' node", buttonType);
			return buttonTextures;
		}
#endif

		if (XMLNode::Attribute attribute = buttonNode.attribute("texture"))
			buttonTextures.defaultTexture = textures->getTexture(attribute->value(), FileManager::Image_UI);

		if (XMLNode::Attribute attribute = buttonNode.attribute("textureSelected"))
			buttonTextures.selected = textures->getTexture(attribute->value(), FileManager::Image_UI);

		if (XMLNode::Attribute attribute = buttonNode.attribute("textureHovering"))
			buttonTextures.hovering = textures->getTexture(attribute->value(), FileManager::Image_UI);

		return buttonTextures;
	}

	static void buildSlider(UISlider* slider, const XMLNode& node)
	{
		const TextureManager* textures = TextureManager::Get();

		// Container
		Texture* container = textures->getTexture(node.attribute("Container")->value(), FileManager::Image_UI);

		// Bar
		UIBox* bar = new UIBox;
		bar->setTexture(textures->getTexture(node.attribute("Bar")->value(), FileManager::Image_UI));
		bar->setParernt(slider);

		// Button
		UIButton* sliderHandle = new UIButton;
		sliderHandle->setParernt(slider);
		if (XMLNode::Attribute attribute = node.attribute("Handle"))
			sliderHandle->setTextures(textures->getTexture(attribute->value(), FileManager::Image_UI), nullptr, nullptr);

		slider->setComponents(container, sliderHandle, bar);
	}


	static UIElement* buildElement(const LayerItem& item)
	{
		switch (item.type)
		{
		case UIType::Element:
		{
			return new UIElement(item.attributes);
		}
		case UIType::Box:
		{
			UIBox* box = new UIBox(item.attributes);

			if (item.attributes.contains("texture"))
			{
				const TextureManager* textures = TextureManager::Get();
				const char* label = item.attributes.at("texture").c_str();
				box->setTexture(textures->getTexture(label, FileManager::Image_UI));

				if (item.attributes.contains("fitting"))
				{
					if (item.attributes.at("fitting") == "clip")
					{
						box->setTextureClipping();
					}
				}
			}

			return box;
		}
		case UIType::Button:
		{
			UIButton* button = new UIButton(item.attributes);

			TexturePacket textures = getButtonTextures(item.attributes.at("texture").c_str());
			button->setTextures(textures.defaultTexture, textures.selected, textures.hovering);

			return button;
		}
		case UIType::Switch:
		{
			UISwitch* uiSwitch = new UISwitch(item.attributes);

			TexturePacket textures = getButtonTextures(item.attributes.at("texture").c_str());
			uiSwitch->setTextures(textures.defaultTexture, textures.selected);
			return uiSwitch;
		}
		case UIType::Slider:
		{
			UISlider* slider = new UISlider(item.attributes);

			XMLParser parser(FileManager::Get()->findFile(FileManager::Configs, "Sliders").c_str());
			XMLNode sliderNode = parser.rootChild(item.attributes.at("slider").c_str());

			buildSlider(slider, sliderNode);

			return slider;
		}
		case UIType::TextBox:
		{
			UITextBox* textBox = new UITextBox(item.attributes, item.value);
			return textBox;
		}
		default:
			break;
		}

		return nullptr;
	}

	static void buildUIScreen(const ScreenItems& screenAttributes, Screen& screen)
	{
		ScreenLayers& screenLayers = screen.layers();

		for (const LayerItems& layerAttributes : screenAttributes)
		{
			ScreenLayer layer("");

			for (const LayerItem& item : layerAttributes)
			{
				UIElement* element = buildElement(item);
				layer.elements.push_back(element);

				if (element->IsInteractable())
					screen.iteractables()[element->id()] = element;
			}

			screenLayers.mLayers.push_back(layer);
		}

		// set up the parent/child heirarchies
		Elements parents = setParents(screen.layers(), screenAttributes);
		Elements elements = screenLayers.elementList();

		for (UIElement* parent : parents)
		{
			for (UIElement* element : elements)
			{
				if (parent == element->parent())
				{
					parent->addChild(element);
				}
			}

			ASSERT(parent->children().size() != 0, "Parent %s could find no children", parent->id());
		}

		// resize/reposition based on child/parent relationships
		for (UIElement* element : elements)
		{
			autoSizeRect(element);

			// update position to be relative to its parent (if it has one)
			RectF rect = element->rect();
			if (element->parent() != nullptr)
			{
				RectF parentRect = element->parent()->rect();
				VectorF parentPosition = parentRect.TopLeft();
				rect.Translate(parentPosition);
			}

			element->setRect(rect);


			// set text sizes
			if (element->type() == UIType::TextBox)
			{
				UITextBox* textBox = static_cast<UITextBox*>(element);

				// If no font size has been given auto size the font to fit the box
				if (!textBox->fontSize())
					textBox->autoSizeFont();

				textBox->align();
			}
		}
	}

	void populateScreen(Screen& screen)
	{
		UIScreen::Type type = screen.type();

		StringBuffer32 fileName;
		type >> fileName;

		FileManager* fm = FileManager::Get();

		ScreenItems attributes;
		readScreen(fm->findFile(FileManager::Config_Menus, fileName.c_str()).c_str(), attributes);
		buildUIScreen(attributes, screen);

#if DEBUG_MODE
		if (screen.layers().mLayers.size() == 0)
			DebugPrint(Warning, "No screen layers built for screen config %s", fm->findFile(FileManager::Config_Menus, fileName.c_str()));
#endif
	}


	/*
	HI samoyed SAUSAGE
	*/

	void readScreen(const char* screen, ScreenItems& screenItems)
	{
		XMLParser parser(screen);
		XMLNode layerNode = parser.rootChild("Layer");
		while (layerNode)
		{
			LayerItems layerItems;

			XMLNode itemNode = layerNode.child();
			while (itemNode)
			{
				LayerItem layerItem;
				layerItem.attributes.fillAtributes(itemNode);
				layerItem.type = GetUIType(itemNode.name());

				if (layerItem.type == UIType::TextBox)
				{
					if (const XMLNode textNode = itemNode.child("text"))
					{
						layerItem.value = textNode.value();
					}
				}

				layerItems.push_back(layerItem);

				itemNode = itemNode.next();
			}

			screenItems.push_back(layerItems);

			layerNode = layerNode.next();
		}
	}
}


Elements ScreenLayers::elementList() const
{
	Elements elements;

	for (const ScreenLayer& screenLayer : mLayers)
	{
		elements.insert(elements.end(), screenLayer.elements.begin(), screenLayer.elements.end());
	}

	return elements;
}

UIElement* ScreenLayers::find(const char* id) const
{
	Elements elements = elementList();

	for (UIElement* element : elements)
	{
		if (StringCompare(element->id(), id))
			return element;
	}

	DebugPrint(Warning, "No element with the id %s was found from the provided screen", id);
	return nullptr;
}

UIElement* ScreenLayers::find(const char* id, UIType type) const
{
	Elements elements = elementList();

	for (UIElement* element : elements)
	{
		if (element->type() == type && StringCompare(element->id(), id))
			return element;
	}

	DebugPrint(Warning, "No element with the id %s was found from the provided screen", id);
	return nullptr;
}

//ScreenLayer* ScreenLayers::layer(const char* id) const
//{
//	for (int i = 0; i < mLayers.size(); i++)
//	{
//		if (mLayers[i].id == id)
//			return &mLayers[i];
//	}
//
//	DebugPrint(Warning, "No layer with the id '%s' exists, returning layer at index 0");
//	return nullptr;
//}