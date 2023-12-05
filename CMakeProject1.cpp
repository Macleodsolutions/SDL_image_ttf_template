#include "CMakeProject1.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load image
    SDL_Surface* loadedSurface = IMG_Load("assets/SDL_logo.png");
    SDL_Texture* texture = NULL;
    if (loadedSurface == NULL) {
        cout << "Unable to load image! SDL_image Error: " << IMG_GetError() << endl;
    }
    else {
        // Create texture from surface
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture == NULL) {
            cout << "Unable to create texture from image! SDL Error: " << SDL_GetError() << endl;
        }
        // Free loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("assets/Roboto-Regular.ttf", 28);
    SDL_Texture* textTexture = NULL;
    SDL_Rect renderQuad = { 0, 0, 0, 0 };
    if (font == NULL) {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
    }
    else {
        // Render text
        SDL_Color textColor = { 255, 255, 255 };  // White color
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Hello, SDL!", textColor);
        if (textSurface != NULL) {
            // Create texture from surface
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture == NULL) {
                cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
            }
            else {
                // Set rendering space
                renderQuad = { 50, 50, textSurface->w, textSurface->h };
            }
            // Free surface
            SDL_FreeSurface(textSurface);
        }
    }

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_RenderClear(renderer);

        // Render texture to screen
        if (texture != NULL) {
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        }

        // Render text to screen
        if (textTexture != NULL) {
            SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        }

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Free resources and close SDL
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    if (textTexture != NULL) {
        SDL_DestroyTexture(textTexture);
    }
    if (font != NULL) {
        TTF_CloseFont(font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
