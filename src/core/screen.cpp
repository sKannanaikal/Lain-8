#include "screen.hpp"

namespace Lain8 {

	Screen::Screen() {
		
		SDL_Init(SDL_INIT_VIDEO);
		this->mWindow = SDL_CreateWindow("Lain8", 640, 320, SDL_WINDOW_RESIZABLE);
		
		this->mRenderer = SDL_CreateRenderer(this->mWindow, nullptr);
		SDL_SetRenderLogicalPresentation(this->mRenderer, 64, 32, SDL_LOGICAL_PRESENTATION_LETTERBOX);
		
		std::uint32_t pixel_buffer[2048];
		std::fill_n(pixel_buffer, 2048, 0xFFFFFFFF);
		this->mSurface = SDL_CreateSurfaceFrom(64, 32, SDL_PIXELFORMAT_RGBA32, pixel_buffer, 256);
		this->mTexture = SDL_CreateTextureFromSurface(this->mRenderer, this->mSurface);
		SDL_SetTextureScaleMode(this->mTexture, SDL_SCALEMODE_NEAREST);

		SDL_RenderClear(this->mRenderer);
		SDL_RenderTexture(this->mRenderer, this->mTexture, nullptr, nullptr);
		SDL_RenderPresent(this->mRenderer);

		SDL_Delay(9000);
	}

	Screen::~Screen() {
		SDL_DestroyWindow(this->mWindow);
		SDL_DestroyRenderer(this->mRenderer);
		SDL_DestroySurface(this->mSurface);
		SDL_DestroyTexture(this->mTexture);
		SDL_Quit();
	}

}
