#include "pch.h"
#include "SColour.h"


SColour::SColour(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

SColour::SColour(int hexValue)
{
	r = (Uint8)((hexValue >> 16) & 0xFF);
	g = (Uint8)((hexValue >> 8) & 0xFF);
	b = (Uint8)((hexValue) & 0xFF);
}

SColour::SColour(Enum colour)
{
	a = c_alphaMax;
	type = colour;

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


SColour::Enum SColour::GetColosestColour() const
{
	Enum closest_colour = Count;
	//int r_diff;
	//int g_diff;
	//int b_diff;
	double total_diff = INT_MAX;

	for( u32 i = 1; i < Count; i++ )
	{
		Enum colour_type = (Enum)i;
		SColour test_colour(colour_type);

		double test_r_diff = r - test_colour.r;
		double test_g_diff = g - test_colour.g;
		double test_b_diff = b - test_colour.b;

		double diff = sqrt( (test_r_diff*test_r_diff) + (test_g_diff*test_g_diff) + (test_b_diff*test_b_diff) );

		if(diff < total_diff)
		{
			closest_colour = colour_type;
			total_diff = diff;
		}
	}

	return closest_colour;
}

void SColour::setOpacity(float opacity)
{
	opacity = Maths::clamp(opacity, 0.0f, 1.0f);
	a = (Uint8)(c_alphaMax * opacity);
}


SDL_Colour SColour::toSDL()
{
	return SDL_Color{ r, g, b };
}