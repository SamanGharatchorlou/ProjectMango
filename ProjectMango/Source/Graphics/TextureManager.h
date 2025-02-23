#pragma once

#include "TextureMap.h"

class STexture;

class TextureManager
{

public:
	static TextureManager* Get();

	void preLoad();
	void load();
	void unload();

	STexture* getTexture(const char* label, const FileManager::Folder folders) const;
	STexture* getTexture(StringBuffer64 label, const FileManager::Folder folders) const
	{
		return getTexture(label.c_str(), folders);
	}

	StringBuffer64 getTextureName(const STexture* texture) const;

private:
	bool loadTexture(TextureMap& textureMap, const char* filePath);
	int loadAllTexturesIn(FileManager::Folder resource_folder, FileManager::Folder placement_folder = FileManager::None);

	std::vector<const TextureMap* > FindTextureMaps(const FileManager::Folder folder) const;

private:
	TextureManager();
	~TextureManager();

	std::unordered_map<FileManager::Folder, TextureMap> mTextures; 
};

