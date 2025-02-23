#pragma once

class STexture;

class LoadingBar //: public Renderable
{
public:
	LoadingBar() : mPercentage(0.0f), mLoadingBar(nullptr), mLoadingBarContainer(nullptr) { }
	~LoadingBar();

	void free();

	void setTextures(STexture* loadingBar, STexture* loadingBarContainer);
	void setRect(RectF rect) { mRect = rect; }

	void setPercentage(float percentage) { mPercentage = percentage; }

	//void render() override;

private:
	float mPercentage;
	RectF mRect;
	STexture* mLoadingBar;
	STexture* mLoadingBarContainer;
};