#ifndef CORE_SCREEN_HPP
#define CORE_SCREEN_HPP

#include <algorithm>
#include <SDL3/SDL.h>
#include <cstdint>

namespace Lain8 {

class Screen {

public:
	SDL_Window *mWindow;
	SDL_Surface *mSurface;
	SDL_Renderer *mRenderer;
	SDL_Texture *mTexture;

	Screen();
	~Screen();
};

}

#endif /* CORE_SCREEN_HPP */
