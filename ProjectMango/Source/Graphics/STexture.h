#pragma once

class STexture
{
public:
	STexture();
	~STexture();

	bool loadFromFile(const BasicString& filePath);

	// NOTE: angle in degress, NOT radians (why?? so annoying)
	void render(const RectF& rect, SDL_RendererFlip flip = SDL_FLIP_NONE, double rotation = 0, VectorF aboutPoint = VectorF());
	void render(const QuadF& quad);
	void renderSubTexture(const RectF& rect, const RectF& subRect, double rotation = 0, VectorF aboutPoint = VectorF(), SDL_RendererFlip flip = SDL_FLIP_NONE);
	
	void SetColourModThisFrame();
	void ResetColourMod();


public:
	VectorF originalDimentions;
	SColour colourModThisFrame;


private:
	// remove renderer - replace with passing in parameter
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};


class TextureGroup
{
public:
	void add(STexture* texture) { mTextures.push_back(texture); }

private:
	std::vector<STexture*> mTextures;
};
