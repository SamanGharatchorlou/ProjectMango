#pragma once

class Texture;

using StringTextureMap = std::unordered_map<StringBuffer64, Texture*>;

class TextureMap
{
public:
	void free();

	void add(const char* id, Texture* texture);

	Texture* texture(const char* id) const { return mData.at(id); }

	StringBuffer64 find(const Texture* texture) const;
	Texture* find(const char* id) const;

	u32 size() const { return (u32)mData.size(); }

private:
	StringTextureMap mData;
};