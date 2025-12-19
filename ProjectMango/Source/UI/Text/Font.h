#pragma once

class Renderer;

struct Font
{
private:
	SDL_Texture* texture = nullptr;

	BasicString fontName;

	// for wrapped text only
	int width = -1;
	bool wrapped = false;

	SDL_Color colour;

	// dont not edit - this is set by the size
	Vector2D<int> size;

	// use SetSize()
	int ptSize = 0;
	

public:
	Font() : texture(nullptr) { }
	~Font();

	void Render(Renderer* renderer, const VectorF position) const;

	// Creates image from font string
	void SetText(const char* _text, bool wrapped = false, int width = -1);
	void SetSize(const char* text, int ptSize);
	void SetColour(const char* text, SDL_Color colour);

	void GetPtSizeForArea(const char* text, VectorF area, int& out_pt_size) const;

	int GetPtSize() const { return ptSize; }
	VectorI GetSize() const { return size; }

	//Font& operator =(const Font& font) = delete;

private:
	void RenderTextSurface(SDL_Surface* textSurface);
	void UpdateText(const char* text);

	// Forbid copy to prevent shared ownership of gFont and
	// it being destroyed by the destructor while still in use
	//Font(Font& font);
	//Font(const Font&) = delete;            // copy constructor
	//Font(Font&&) noexcept = delete;        // move constructor
	//Font& operator=(Font&) noexcept = delete;  // move assignment

	//Font(const Font&) = delete;
	//Font& operator=(const Font&) = delete;
};
 