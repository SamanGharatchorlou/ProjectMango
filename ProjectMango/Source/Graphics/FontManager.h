#pragma once

struct FontManager
{	
	struct Key
	{
		BasicString name;
		int ptSize;
		    
		bool operator==(const Key& other) const
		{
			return ptSize == other.ptSize && name == other.name;
		}
	};

	// define the hash for the font key
	struct FontKeyHash
	{
		size_t operator()(const Key& key) const
		{
			// use the preexisting BasicString hash
			size_t h1 = std::hash<BasicString>()(key.name);
			size_t h2 = std::hash<int>()(key.ptSize);

			return h1 ^ (h2 << 1);
		}
	};
	
	~FontManager();

	TTF_Font* GetFont(const char* font_key, int pt_side);



	std::unordered_map<Key, TTF_Font*, FontKeyHash> fonts;
};