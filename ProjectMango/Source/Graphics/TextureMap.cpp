#include "pch.h"
#include "TextureMap.h"
#include "Texture.h"


void TextureMap::free()
{
	StringTextureMap::iterator iter = mData.begin();
	for (; iter != mData.end(); iter++)
	{
		if (iter->second)
		{
			delete iter->second;
			iter->second = nullptr;
		}
	}

	mData.clear();
}

void TextureMap::add(const char* id, Texture* texture)
{
#if DEBUG_CHECK
	if (mData.count(id) > 0)
	{
		DebugPrint(Warning, "ID '%s' already contained in texture map. Replacing old value, this value might be lost forever(and a memory leak)", id);
	}
#endif
	mData[id] = texture; 
}


StringBuffer32 TextureMap::find(const Texture* texture) const
{
	StringTextureMap::const_iterator iter;
	for (iter = mData.begin(); iter != mData.end(); iter++)
	{
		if (texture == iter->second)
		{
			return iter->first;
		}
	}

	return StringBuffer32();
}


Texture* TextureMap::find(const char* id) const 
{ 
 	StringTextureMap::const_iterator iter = mData.find(id);
	return iter != mData.end() ? iter->second : nullptr;
}