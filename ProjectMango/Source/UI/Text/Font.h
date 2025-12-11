#pragma once

class Renderer;

struct Font
{
	Renderer* renderer;
	SDL_Texture* texture;

	TTF_Font* ttfFont;
	StringBuffer64 fontName;

	BasicString text;

	SDL_Color colour;

	// for wrapped text only
	int width = -1;
	bool wrapped = false;

private:
	// this is set by the size
	Vector2D<int> size;

	// use Resize()
	int ptSize = 0;
	

public:
	Font() : texture(nullptr), ttfFont(nullptr), renderer(nullptr) { }
	~Font();

	bool LoadFromFile(const char* filePath, int ptSize);
	void Render(const VectorF position) const;

	// Creates image from font string
	void SetText(const char* _text, bool wrapped = false, int width = -1);
	void Resize(int ptSize);

	int GetPtSize() const { return ptSize; }
	VectorI GetSize() const { return size; }

	Font& operator =(const Font& font);

private:
	void RenderTextSurface(SDL_Surface* textSurface);

	// Forbid copy to prevent shared ownership of gFont and
	// it being destroyed by the destructor while still in use
	//Font(Font& font);
	Font(const Font&);            // copy constructor
	Font(Font&&) noexcept;        // move constructor
	Font& operator=(Font&&) noexcept;  // move assignment
};
 