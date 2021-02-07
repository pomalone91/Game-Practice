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
#include <stdbool.h>

#define GRAVITY     0.35f

typedef struct
{
    float x, y; // position of block
    float dy;   // Vertical velocity of player. Change in position applied once per frame
    short life;
    char *name;
    bool onLedge;
} Man;

typedef struct
{
    int x, y;
} Star;

typedef struct
{
    int x, y, w, h;
} Ledge;

typedef struct
{
    // Player
    Man man;
    
    Star stars[100];
    
    // Images
    SDL_Texture *star;
    
    // Ledges
    Ledge ledges[100];
    
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
    for (int i = 0; i < 100; i++)
    {
        gameState->stars[i].x = random()%640; //i*64;
        gameState->stars[i].y = random()%480; //i*32;
    }
    
    // Init ledges
    for (int i = 0; i < 100; i++)
    {
        gameState->ledges[i].w = 256;
        gameState->ledges[i].h = 64;
        gameState->ledges[i].x = i * 256;
        gameState->ledges[i].y = 400;
    }
    gameState->ledges[99].x = 350;
    gameState->ledges[99].y = 200;
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
//                    case SDLK_UP:
//                        if (gamestate->man.dy == 0.0f)
//                        {
//                            gamestate->man.dy = -15;
//                        }
//                        break;
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
        gamestate->man.x -= 10;
    
    if (state[SDL_SCANCODE_RIGHT])
        gamestate->man.x += 10;
    if (state[SDL_SCANCODE_UP] && gamestate->man.onLedge == true) {
        gamestate->man.dy = -12;
        gamestate->man.onLedge = false;
    }
//    if (state[SDL_SCANCODE_DOWN]) {
//        gamestate->man.y += 10;
//    }
    
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
    SDL_Rect rect = { gamestate->man.x, gamestate->man.y, 48, 48 };
    SDL_RenderFillRect(renderer, &rect);
    
//    for (int i = 0; i < 100; i++) {
//        // Draw the image
//        SDL_Rect beetleRect = { gamestate->stars[i].x, gamestate->stars[i].y, 64, 64 };
//        SDL_RenderCopy(renderer, gamestate->star, NULL, &beetleRect);
//    }
    
    // Draw Ledges
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    for (int i = 0; i < 100; i++) {
        SDL_Rect ledgeRect = { gamestate->ledges[i].x, gamestate->ledges[i].y, gamestate->ledges[i].w, gamestate->ledges[i].h };
        SDL_RenderFillRect(renderer, &ledgeRect);
    }

    // We're done drawing, present what has been drawn
    SDL_RenderPresent(renderer);
}

void process(Gamestate *game)
{
    // Every frame we need to apply the velocity to the man's position. Then we need to apply gravity to the velocity.
    Man *man = &game->man;
    man->y += man->dy;
    man->dy += GRAVITY;
}

void collisionDetect(Gamestate *game)
{
    // Check for collisions between man and brick ledges
    for (int i = 0; i < 100; i++)
    {
        // Man dimensions and position
        float mw = 48, mh = 48;
        float mx = game->man.x;
        float my = game->man.y;
        
        // Ledge dimensions and position
        float bx = game->ledges[i].x;
        float by = game->ledges[i].y;
        float bw = game->ledges[i].w;
        float bh = game->ledges[i].h;
        
        if (my+mh > by && my < by+bh) {     // Check if we're at the same height as a ledge
            // If the man is standing to the right of the brick but his width is overlapping it
            if (mx < bx+bw && mx+mw > bx+bw && !game->man.onLedge) {
                // Correct x position
                game->man.x = bx+bw;
                mx = bx+bw;
            } else if (mx+mw > bx && mx < bx && !game->man.onLedge)   // If man is standing to the left of the ledge check if there is overlap
            {
                game->man.x = bx-mw;
                mx = bx-mw;
            }
        }
        
        if (mx+mw > bx && mx < bx+bw) {     // Check if we are at the same horizontal position as a ledge
            // Are we bumping our head?
            if (my < by+bh && my > by) {
                // Correct y position
                game->man.y = by+bh;
                
                // Stop jump velocity
                game->man.dy = 0.0f;
            } else if (my+mh > by && my < by)   // Check if our feet are overlapping the ledge
            {
                // Correct y position
                game->man.y = by-mh;
                
                // Stop jump velocity
                game->man.dy = 0.0f;
                game->man.onLedge = true;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int processEvents(SDL_Window *window, Gamestate *gamestate);
    void doRender(SDL_Renderer *renderer, Gamestate *gamestate);
    void loadGame(Gamestate *gameState);
    void collisionDetect(Gamestate *game);
    
    
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
        
        // Process gamestate
        process(&gamestate);
        
        // Detect collisions
        collisionDetect(&gamestate);
        
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
