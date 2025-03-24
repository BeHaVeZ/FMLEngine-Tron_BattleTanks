#pragma once

#include <SDL.h>
#include <string>

namespace FML
{

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool LoadFromFile(SDL_Renderer* renderer, std::string path);

		void Free();

		void Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr);

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }


	private:
		SDL_Texture* m_Texture;
		int m_Width;
		int m_Height;
	};

}

