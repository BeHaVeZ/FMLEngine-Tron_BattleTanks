#include "Texture.h"
#include <SDL_image.h>
#include <stdio.h>

namespace FML
{

	Texture::Texture() : m_Texture(nullptr), m_Width(0), m_Height(0) {}
	Texture::~Texture() {
		Free();
	}

	bool Texture::LoadFromFile(SDL_Renderer* renderer, std::string path) {
		Free();

		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr) {
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
			return false;
		}

		m_Texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (m_Texture == nullptr) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			SDL_FreeSurface(loadedSurface);
			return false;
		}

		m_Width = loadedSurface->w;
		m_Height = loadedSurface->h;

		SDL_FreeSurface(loadedSurface);

		return true;
	}

	void Texture::Free() {
		if (m_Texture != nullptr) {
			SDL_DestroyTexture(m_Texture);
			m_Texture = nullptr;
			m_Width = 0;
			m_Height = 0;
		}
	}

	void Texture::Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
		SDL_Rect renderQuad = { x, y, m_Width, m_Height };

		if (clip != nullptr) {
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopy(renderer, m_Texture, clip, &renderQuad);
	}


}

