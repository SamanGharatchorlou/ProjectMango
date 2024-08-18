#pragma once

#include "TextureMap.h"

class Texture;

class TextureManager
{

public:
	static TextureManager* Get();

	void preLoad();
	void load();
	void unload();

	Texture* getTexture(const char* label, const FileManager::Folder folders) const;
	Texture* getTexture(StringBuffer64 label, const FileManager::Folder folders) const
	{
		return getTexture(label.c_str(), folders);
	}

	StringBuffer64 getTextureName(const Texture* texture) const;

private:
	bool loadTexture(TextureMap& textureMap, const char* filePath);
	int loadAllTexturesIn(FileManager::Folder resource_folder, FileManager::Folder placement_folder = FileManager::None);

	const TextureMap* findTextureMap(const FileManager::Folder folder) const;

	Texture* searchAllFiles(const char* label) const;

private:
	TextureManager();
	~TextureManager();

	std::unordered_map<FileManager::Folder, TextureMap> mTextures; 
};

