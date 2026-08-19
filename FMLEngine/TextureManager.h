#pragma once
#include "Singleton.h"
#include <SDL.h>
#include <string>
#include <map>

namespace FML
{

	class TextureManager : public Singleton<TextureManager> {
	public:
		bool Load(const std::string& id, const std::string& filename, SDL_Renderer* renderer);

		SDL_Texture* GetTexture(const std::string& id);

		void Clear();

	private:
		friend class Singleton<TextureManager>;
		TextureManager() {}
		~TextureManager() {
			Clear();
		}

		std::map<std::string, SDL_Texture*> textureMap;
	};

}
