#include "pch.h"
#include "Font.h"

#include "Graphics/Renderer.h"

Font::~Font()
{
	if (texture)
		SDL_DestroyTexture(texture);

	if(ttfFont)
		TTF_CloseFont(ttfFont);
}

Font& Font::operator =(const Font& font) 
{ 
	// cant copy these over, generate new ones
	renderer = nullptr;
	texture = nullptr;
	ttfFont = nullptr;

	fontName = font.fontName;
	colour = font.colour;
	ptSize = font.ptSize;
	text = font.text;

	// we have to own the ttfFont since we clean it up ourselves
	BasicString path = FileManager::Get()->findFile(FileManager::Font, fontName.c_str());
	LoadFromFile(path.c_str(), ptSize);
	SetText(text.c_str(), wrapped, width);

	return *this;
};

bool Font::LoadFromFile(const char* filePath, int _ptSize)
{
	renderer = Renderer::Get();

	//Open the font
	ttfFont = TTF_OpenFont(filePath, ptSize);
	if (ttfFont != nullptr)
	{
		fontName = FileManager::Get()->getItemName(fs::path(filePath));
		ptSize = _ptSize;
		return true;
	}
	else
	{
		DebugPrint(Warning, "Failed to load font at '%s'! SDL_ttf Error: %s", filePath, TTF_GetError());
		return false;
	}
}

void Font::Resize(int _ptSize)
{
	// Font must be closed and reloaded
	TTF_CloseFont(ttfFont);

	BasicString font_file = FileManager::Get()->findFile(FileManager::Font, fontName.c_str());
	ttfFont = TTF_OpenFont(font_file.c_str(), _ptSize);
	ptSize = _ptSize;
}

void Font::SetText(const char* _text, bool _wrapped, int _width)
{
	if(_text)
	{
		text = _text;
		wrapped = _wrapped;
		width = _width;

		if ( ttfFont != nullptr )
		{
			// Render text surface
			SDL_Surface* textSurface = nullptr;
		
			if(wrapped)
				textSurface = TTF_RenderText_Blended_Wrapped(ttfFont, text.c_str(), colour, width);
			else
				textSurface = TTF_RenderText_Blended(ttfFont, text.c_str(), colour);

			if ( textSurface )
			{
				RenderTextSurface(textSurface);
			}
			else
			{
				DebugPrint(Warning, "Unable to render text surface for text: %s! SDL_ttf Error: %s", text.c_str(), TTF_GetError());
			}
		}
		else
		{
			if(ttfFont == nullptr)
				DebugPrint(Error, "Font has not beed loaded for text: %s, Call Font::loadFromFile first", text);
		}
	}
}

void Font::Render(const VectorF position) const
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

	//Create texture from surface pixels
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