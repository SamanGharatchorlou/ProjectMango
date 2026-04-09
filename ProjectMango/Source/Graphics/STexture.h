#pragma once

struct SDL_Renderer;

struct STexture
{
public:
	~STexture();

	bool loadFromFile(const BasicString& filePath, SDL_Renderer* renderer);

	// NOTE: angle in degress, NOT radians (why?? so annoying)
	void render(SDL_Renderer* renderer, const RectF& rect, SDL_RendererFlip flip = SDL_FLIP_NONE, double rotation = 0, VectorF aboutPoint = VectorF());
	void render(SDL_Renderer* renderer, const QuadF& quad);
	void renderSubTexture(SDL_Renderer* renderer, const RectF& rect, const RectF& subRect, double rotation = 0, VectorF aboutPoint = VectorF(), SDL_RendererFlip flip = SDL_FLIP_NONE);
	
	void SetColourModThisFrame();
	void ResetColourMod();

	VectorF originalDimentions;
	SColour colourModThisFrame;

	SDL_Texture* texture = nullptr;
};
