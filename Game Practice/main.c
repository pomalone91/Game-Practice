//
//  main.c
//  Game Practice
//
//  Created by Paul Malone on 2/3/21.
//  Copyright © 2021 Paul Malone. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
  int x, y;
  short life;
  char *name;
} Man;

typedef struct
{
    int x, y;
} Star;

typedef struct
{
    // Player
    Man man;
    
    Star stars[100];
    
    // Images
    SDL_Texture *star;
    
    SDL_Renderer *renderer;
    
} Gamestate;

void loadGame(Gamestate *gameState)
{
    gameState->man.x = 220;
    gameState->man.y = 140;
    
    SDL_Surface *starSurface;             // Declare a surface for a texture
    
    // Load images and create rendering texture
    starSurface = IMG_Load("star.png");
    if (starSurface == NULL) {
        printf("Couldn't find star sprite\n");
        SDL_Quit();
        exit(1);
    }
    
    // Add image to gamestate
    gameState->star = SDL_CreateTextureFromSurface(gameState->renderer, starSurface);
    SDL_FreeSurface(starSurface);
    
    // Set up star images
    for (int i = 0; i < 100; i++) {
        gameState->stars[i].x = random()%640; //i*64;
        gameState->stars[i].y = random()%480; //i*32;
    }
}

int processEvents(SDL_Window *window, Gamestate *gamestate)
{
    SDL_Event event;
    int done = 0;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT_CLOSE:         // Close the window, remove reference for window pointer
                if (window) {
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done = 1;
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:           // Quit out if they press the escape key. Right now this switch doesn't do anything else
                        done = 1;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:                      // Quit the game
                done = 1;
                break;
            default:
                break;
        }
    }
    
    // Check key states for continuous movement
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if ( state[SDL_SCANCODE_LEFT] )
    {
        gamestate->man.x -= 10;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        gamestate->man.x += 10;
    }
    if (state[SDL_SCANCODE_UP]) {
        gamestate->man.y -= 10;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        gamestate->man.y += 10;
    }
    
    return done;
}

void doRender(SDL_Renderer *renderer, Gamestate *gamestate)
{
    // Set renderer draw color
    SDL_SetRenderDrawColor(renderer, 100, 55, 255, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Draw a white rectangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Create SDL_RECT  x    y    w    h
    // Uses coordinates of man object for where to draw
    SDL_Rect rect = { gamestate->man.x, gamestate->man.y, 80, 80 };
    SDL_RenderFillRect(renderer, &rect);
    
    for (int i = 0; i < 100; i++) {
        // Draw the image
        SDL_Rect beetleRect = { gamestate->stars[i].x, gamestate->stars[i].y, 64, 64 };
        SDL_RenderCopy(renderer, gamestate->star, NULL, &beetleRect);
    }

    // We're done drawing, present what has been drawn
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    int processEvents(SDL_Window *window, Gamestate *gamestate);
    void doRender(SDL_Renderer *renderer, Gamestate *gamestate);
    void loadGame(Gamestate *gameState);
    
    SDL_Window *window;                    // Declare a window
    SDL_Renderer *renderer;                // Declare a renderer
    
    
    // Declare man object
    Gamestate gamestate;

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
    
    // Initialize random number seed
    srandom((int)time(NULL));
    

    //Create an application window with the following settings:
    window = SDL_CreateWindow("Game Window",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            640,                               // width, in pixels
                            480,                               // height, in pixels
                              0);                                  // flags
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    gamestate.renderer = renderer;
    
    // Load the game
    loadGame(&gamestate);
    
    // Set up event loop
    int done = 0;
//    SDL_Event event;
    
    // Enter the event loop
    while (!done)
    {
        // Check for SDL events
        done = processEvents(window, &gamestate);
        
        // Do render stuff
        doRender(renderer, &gamestate);
    
        // Wait 2 seconds
//        SDL_Delay(10);
        
    }

    // Close and destroy the window
    SDL_DestroyTexture(gamestate.star);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    // Clean up
    SDL_Quit();
    return 0;
}
