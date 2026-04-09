#pragma once

struct STexture;

using StringTextureMap = std::unordered_map<StringBuffer64, STexture*>;

class TextureMap
{
public:
	void free();

	void add(const char* id, STexture* texture);

	STexture* texture(const char* id) const { return mData.at(id); }

	StringBuffer64 find(const STexture* texture) const;
	STexture* find(const char* id) const;

	u32 size() const { return (u32)mData.size(); }

private:
	StringTextureMap mData;
};