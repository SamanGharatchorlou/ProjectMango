#pragma once

constexpr Uint8 alphaMax = 255;
constexpr Uint8 alphaMin = 0;

struct Colour
{
	enum Enum
	{
		None,
		Red,
		Blue,
		Purple,
		Green,
		Yellow,
		LightGrey,
		Black,
		White,
	};

	Colour() : r(0), g(0), b(0), a(alphaMax) { }
	Colour(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = alphaMax);
	Colour(Enum colour);

	void setOpacity(float opacity);

	SDL_Colour toSDL();

	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};
