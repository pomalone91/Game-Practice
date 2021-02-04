//
//  main.c
//  Game Practice
//
//  Created by Paul Malone on 2/3/21.
//  Copyright © 2021 Paul Malone. All rights reserved.
//

#import <SDL2/SDL.h>
#include <stdio.h>

typedef struct
{
  int x, y;
  short life;
  char *name;
} Man;

int processEvents(SDL_Window *window, Man *man)
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
        man->x -= 10;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        man->x += 10;
    }
    if (state[SDL_SCANCODE_UP]) {
        man->y -= 10;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        man->y += 10;
    }
    
    return done;
}

void doRender(SDL_Renderer *renderer, Man *man)
{
    // Set renderer draw color
    SDL_SetRenderDrawColor(renderer, 100, 55, 255, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Draw a white rectangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Create SDL_RECT  x    y    w    h
    // Uses coordinates of man object for where to draw
    SDL_Rect rect = { man->x, man->y, 200, 200};
    SDL_RenderFillRect(renderer, &rect);

    // We're done drawing, present what has been drawn
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    int processEvents(SDL_Window *window, Man *man);
    void doRender(SDL_Renderer *renderer, Man *man);
    
    SDL_Window *window;                    // Declare a window
    SDL_Renderer *renderer;                // Declare a renderer

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
    
    // Declare man object
    Man man;
    man.x = 220;
    man.y = 140;

    //Create an application window with the following settings:
    window = SDL_CreateWindow("Game Window",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            640,                               // width, in pixels
                            480,                               // height, in pixels
                              0);                                  // flags
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Set up event loop
    int done = 0;
//    SDL_Event event;
    
    // Enter the event loop
    while (!done)
    {
        // Check for SDL events
        done = processEvents(window, &man);
        
        // Do render stuff
        doRender(renderer, &man);
    
        // Wait 2 seconds
        SDL_Delay(10);
        
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    // Clean up
    SDL_Quit();
    return 0;
}
