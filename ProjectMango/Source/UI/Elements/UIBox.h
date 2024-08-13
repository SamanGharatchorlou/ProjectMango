#pragma once

#include "UIElement.h"

class Texture;
class InputManager;

class UIBox : public UIElement
{
public:
	UIBox();
	UIBox(const StringMap32& attributes);
	virtual ~UIBox() { }

	void fill(const StringMap32& attributes);

	void setTexture(Texture* texture) { mTexture = texture; }

	// TODO: make this...better
	// Keeps the original ratio, clipping the texture if required
	void setTextureClipping();

	const Texture* texture() const { return mTexture; }

	bool isPointInBounds(VectorF point);

	virtual void render() override;
	virtual void render(RectF subRect);

	virtual UIType type() const override { return UIType::Box; }

protected:
	Texture* mTexture;

	RectF mSubrect;

#if DEBUG_CHECK
	bool mDrawRect;
#endif
};
