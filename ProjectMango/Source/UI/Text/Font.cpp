#include "pch.h"
#include "Font.h"

#include "Graphics/Renderer.h"
#include "Graphics/FontManager.h"

static constexpr int c_defaultSize = 10;
static constexpr const char* c_defaultFont = "convent";


Font::~Font()
{
	if (texture)
		SDL_DestroyTexture(texture);

	texture = nullptr;
}
 
void Font::SetSize(const char* text, int _ptSize)
{
	ptSize = _ptSize;
	UpdateText(text);
}

void Font::SetText(const char* text, bool _wrapped, int _width)
{
	wrapped = _wrapped;
	width = _width;
	UpdateText(text);
}

void Font::SetColour(const char* text, SDL_Color _colour)
{
	colour = _colour;
	UpdateText(text);
}

void Font::Render(Renderer* renderer, const VectorF position) const
{
	SDL_Rect renderQuad = { static_cast<int>(position.x),
							static_cast<int>(position.y),
							size.x, size.y };

	SDL_RenderCopyEx(renderer->sdlRenderer(), texture, nullptr, &renderQuad, 0.0, NULL, SDL_FLIP_NONE);
}

// -- Private Functions -- //
void Font::RenderTextSurface(SDL_Surface* textSurface)
{
	if (texture)
		SDL_DestroyTexture(texture);

	// create texture from surface pixels
	Renderer* renderer = Renderer::Get();
	renderer->lock();
	texture = SDL_CreateTextureFromSurface(renderer->sdlRenderer(), textSurface);
	renderer->unlock();

	if (texture == nullptr)
		DebugPrint(Warning, "Unable to create texture from rendered text! SDL Error: %s", SDL_GetError());
	else
		size = Vector2D<int>(textSurface->w, textSurface->h);

	// loaded surface no longer needed
	SDL_FreeSurface(textSurface);
	textSurface = nullptr;
}

void Font::GetPtSizeForArea(const char* text, VectorF area, int& out_pt_size) const
{
	int width = -1;
	int height = -1;

	int targetWidth = (int)area.x;
	int targetHeight = (int)area.y;

	out_pt_size;
	TTF_Font* ttf_font = GameData::Get().fontManager->GetFont(fontName.c_str(), out_pt_size);

	int error = TTF_SizeText(ttf_font, text, &width, &height);
	if(error == -1)
	{
		DebugPrint(Warning, "Cannot resize font: %s", fontName.c_str());
		return;
	}

	if (width > targetWidth || height > targetHeight)
	{
		while (width > targetWidth || height > targetHeight)
		{
			//SetSize(text, --out_pt_size);
			ttf_font = GameData::Get().fontManager->GetFont(fontName.c_str(), --out_pt_size);
			TTF_SizeText(ttf_font, text, &width, &height);
		}
	}
	else
	{
		while (width < targetWidth && height < targetHeight)
		{
			//SetSize(text, ++out_pt_size);
			ttf_font = GameData::Get().fontManager->GetFont(fontName.c_str(), ++out_pt_size);
			TTF_SizeText(ttf_font, text, &width, &height);
		}
	}
}

void Font::UpdateText(const char* text)
{
	if(!text || strlen(text) == 0)
		return;

	if(ptSize == 0)
		ptSize = c_defaultSize;

	if(fontName.empty())
		fontName = c_defaultFont;

	TTF_Font* ttf_font = GameData::Get().fontManager->GetFont(fontName.c_str(), ptSize);

	// render text surface
	SDL_Surface* textSurface = nullptr;
		
	if(wrapped)
		textSurface = TTF_RenderText_Blended_Wrapped(ttf_font, text, colour, width);
	else
		textSurface = TTF_RenderText_Blended(ttf_font, text, colour);

	if ( textSurface )
	{
		RenderTextSurface(textSurface);
	}
	else
	{
		DebugPrint(Warning, "Unable to render text surface for text: %s! SDL_ttf Error: %s", text, TTF_GetError());
	}
}