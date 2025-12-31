#include "pch.h"
#include "FontManager.h"

FontManager::~FontManager()
{
	for( auto iter = fonts.begin(); iter != fonts.end(); iter++ )
	{
		TTF_CloseFont(iter->second);
	}

	fonts.clear();
}

TTF_Font* FontManager::GetFont(const char* font_key, int pt_size)
{
	Key key;
	key.name = BasicString::Ref(font_key);
	key.ptSize = pt_size;
	if(fonts.contains( key ))
	{
		return fonts.at( key );
	}
	else
	{
		// open the font
		BasicString path;
		FileManager::Get()->FindFile(FileManager::Font, font_key, path);
		if (TTF_Font* ttf_font = TTF_OpenFont(path.c_str(), pt_size))
		{
			fonts.insert( { { BasicString(font_key), pt_size }, ttf_font } );
			return ttf_font;
		}

		DebugPrint(Warning, "Failed to load font at '%s'! SDL_ttf Error: %s", path.c_str(), TTF_GetError());
		return nullptr;
	}
}
