#include "pch.h"
#include "Colour.h"


Colour::Colour(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}


Colour::Colour(Enum colour)
{
	a = alphaMax;

	switch (colour)
	{
	case None:
		r = 0; g = 0; b = 0;
		break;
	case Red:
		r = 255; g = 0; b = 0;
		break;
	case Blue:
		r = 0; g = 0; b = 255;
		break;
	case Purple:
		r = 128; g = 0; b = 128;
		break;
	case Green:
		r = 0; g = 255; b = 0;
		break;
	case Yellow:
		r = 255; g = 255; b = 0;
		break;
	case LightGrey:
		r = 201; g = 201; b = 201;
		break;
	case MidGrey:
		r = 125; g = 125; b = 125;
		break;
	case Black:
		r = 0; g = 0; b = 0;
		break;
	case White:
		r = 255; g = 255; b = 255;
		break;
	default:
		r = 0; g = 0; b = 0;
		DebugPrint(Warning, "Render colour has not been defined");
		break;
	}
}

void Colour::setOpacity(float opacity)
{
	opacity = Maths::clamp(opacity, 0.0f, 1.0f);
	a = (Uint8)(alphaMax * opacity);
}


SDL_Colour Colour::toSDL()
{
	return SDL_Color{ r, g, b };
}