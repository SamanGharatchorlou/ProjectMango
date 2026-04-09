#include "pch.h"
#include "STexture.h"
#include "Renderer.h"

STexture::~STexture()
{
	if (texture)
	{
		SDL_DestroyTexture(texture);
	}
}

bool STexture::loadFromFile(const BasicString& filePath, SDL_Renderer* renderer)
{
	// remove any existing texture
	if (texture)
	{
		SDL_DestroyTexture(texture);
	}

	// load image
	SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());

	if (!loadedSurface)
	{
		DebugPrint(Warning, "Unable to load image %s! SDL Image Error: %s", filePath.c_str(), IMG_GetError());
	}
	else
	{
		// create texture from surface pixels
		if (!renderer)
		{
			DebugPrint(Warning, "No renderer has been attached to texture: %s", filePath.c_str());
		}
		else
		{
			texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

			// set image dimentions
			if (!texture)
			{
				DebugPrint(Warning, "Unable to create texture from %s! SDL Error: %s", filePath.c_str(), SDL_GetError());
			}
			else
			{
				originalDimentions = VectorF(static_cast<float>(loadedSurface->w), static_cast<float>(loadedSurface->h));
			}
		}

		// loaded surface no longer needed
		SDL_FreeSurface(loadedSurface);
	}

	// return sucess
	return texture != nullptr;
}

	
void STexture::SetColourModThisFrame()
{
	// Apply temporary colour modulation Colour
	int col_success = SDL_SetTextureColorMod(texture, colourModThisFrame.r, colourModThisFrame.g, colourModThisFrame.b);
	if (col_success == -1)
		DebugPrint(PriorityLevel::Log, "%s", SDL_GetError());

	// Apply temporary alpha modulation Colour
	int alp_success = SDL_SetTextureAlphaMod(texture, colourModThisFrame.a);
	if (alp_success == -1)
		DebugPrint(PriorityLevel::Log, "%s", SDL_GetError());
}

void STexture::ResetColourMod()
{
	// reset the colour mod each frame
	colourModThisFrame = SColour();
	SDL_SetTextureColorMod(texture, colourModThisFrame.r, colourModThisFrame.g, colourModThisFrame.b);
	SDL_SetTextureAlphaMod(texture, colourModThisFrame.a);
}

// Renders texture with the roation specified
// NOTE: the about point is relative to the rect e.g. about the center would be rect.size()/2, not rect.center()
void STexture::render(SDL_Renderer* renderer, const RectF& rect, SDL_RendererFlip flip, double rotation, VectorF aboutPoint)
{
	SDL_Rect renderQuad = rect.toSDLRect();
	SDL_Point point = { (int)(aboutPoint.x + 0.5f), (int)(aboutPoint.y + 0.5f) };
		
	// Apply temporary colour modulation Colour
	SetColourModThisFrame();

	SDL_RenderCopyEx(renderer, texture, nullptr, &renderQuad, rotation, &point, flip);

	// reset the colour mod each frame
	ResetColourMod();
}

// Renders part of the texture, e.g. a tile in a set with the roation specified
void STexture::renderSubTexture(SDL_Renderer* renderer, const RectF& rect, const RectF& subRect, double rotation, VectorF aboutPoint, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = rect.toSDLRect();
	SDL_Rect subQuad = subRect.toSDLRect();

	// rotate about this point
	SDL_Point point = aboutPoint.toSDLPoint();

	// Apply temporary colour modulation Colour
	SetColourModThisFrame();

	SDL_RenderCopyEx(renderer, texture, &subQuad, &renderQuad, rotation, &point, flip);

	// reset the colour mod each frame
	ResetColourMod();
}

// Render quad with an aboutpoint set.
void STexture::render(SDL_Renderer* renderer, const QuadF& quad)
{
	SDL_Rect renderQuad = quad.getRect().toSDLRect();
	SDL_Point point = quad.aboutPoint().toSDLPoint();

	// Apply temporary colour modulation Colour
	SetColourModThisFrame();

	SDL_RenderCopyEx(renderer, texture, nullptr, &renderQuad, quad.rotation(), &point, SDL_FLIP_NONE);

	// reset the colour mod each frame
	ResetColourMod();
}
