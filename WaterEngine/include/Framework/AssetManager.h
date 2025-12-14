#pragma once
#include <SFML/Graphics.hpp>
#include "Framework/Core.h"

namespace we
{
	class AssetManager
	{
	public:
		static AssetManager& GetAssetManager();
		shared<sf::Texture> LoadTexture(const string& TexturePath);
		shared<sf::Font> LoadFont(const string& FontPath);
		void GarbageCollectionCycle();
		void SetAssetRootDirctory(const string& Directory);

	protected:
		AssetManager();

	private:
		static unique<AssetManager> UAssetManager;
		Dictionary<string, shared<sf::Texture>> LoadedTextures;
		Dictionary<string, shared<sf::Font>> LoadedFonts;
		string RootDirectory;
	};
}