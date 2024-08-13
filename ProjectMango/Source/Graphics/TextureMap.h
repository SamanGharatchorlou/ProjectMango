#pragma once

class Texture;

using StringTextureMap = std::unordered_map<StringBuffer32, Texture*>;

class TextureMap
{
public:
	void free();

	void add(const char* id, Texture* texture);

	Texture* texture(const char* id) const { return mData.at(id); }

	StringBuffer32 find(const Texture* texture) const;
	Texture* find(const char* id) const;

	int size() const { return mData.size(); }

private:
	StringTextureMap mData;
};