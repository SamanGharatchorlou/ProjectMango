#pragma once

constexpr Uint8 c_rgbMax = 255;
constexpr Uint8 c_alphaMax = 255;
constexpr Uint8 c_alphaMin = 0;

struct SColour
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
		MidGrey,
		Black,
		White,

		Custom,
		Count
	};

	SColour() { }
	SColour(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = c_alphaMax);
	SColour(Enum colour);
	SColour(int hexValue);

	void setOpacity(float opacity);

	Enum GetColosestColour() const;
	int GetHex() const { return (r << 16) | (g << 8) | b; }

	SDL_Colour toSDL();

	Uint8 r = c_rgbMax;
	Uint8 g = c_rgbMax;
	Uint8 b = c_rgbMax;
	Uint8 a = c_alphaMax;

	Enum type = Count;
};
