#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool loadFromFile(const BasicString& filePath);

	// NOTE: angle in degress, NOT radians (why?? so annoying)
	void render(const RectF& rect, SDL_RendererFlip flip = SDL_FLIP_NONE, double rotation = 0, VectorF aboutPoint = VectorF());
	void renderSubTexture(const RectF& rect, const RectF& subRect, double rotation = 0, VectorF aboutPoint = VectorF(), SDL_RendererFlip flip = SDL_FLIP_NONE);

	void render(const QuadF& quad) const;

	void SetColourModThisFrame(Colour colour) { colourModThisFrame = colour; }

public:
	VectorF originalDimentions;
	Colour colourModThisFrame;


private:
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};


class TextureGroup
{
public:
	void add(Texture* texture) { mTextures.push_back(texture); }

private:
	std::vector<Texture*> mTextures;
};
