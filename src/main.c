#define TRUE 1
#define FALSE 0
#define CELLS 64

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include "random.c"
#include "structs.c"
#include "collision.c"
#include "config.c"

#define randnum(min, max) \
    ((rand() % (int)(((max) + 1) - (min))) + (min))

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(void)
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_AUDIO); 
    //Initialize SDL_ttf
    if(TTF_Init() == -1)
    {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    //success = false;
    }
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        //            success = false;
    }

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* win = SDL_CreateWindow("Collect mushrooms", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
    
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
    
    //grass load
	SDL_Surface* bmp = SDL_LoadBMP("resource/game/img/grass.bmp");
	if (bmp == NULL) {
		fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_FreeSurface(bmp);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	SDL_FreeSurface(bmp);
    
    //player load
	SDL_Surface* bmpHero = SDL_LoadBMP("resource/game/img/hero.bmp");
	if (bmpHero == NULL) {
		fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
    
    SDL_Texture* texHero = SDL_CreateTextureFromSurface(ren, bmpHero);
	if (texHero == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_FreeSurface(bmpHero);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

    //mushroom load
	SDL_Surface* bmpMushroom = SDL_LoadBMP("resource/game/img/mushr_broth.bmp");
    SDL_Surface* bmpMushroomMenu = SDL_LoadBMP("resource/game/img/mushr_broth.bmp");

	if (bmpMushroom == NULL) {
		fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}    
    SDL_Texture* texMushroom = SDL_CreateTextureFromSurface(ren, bmpMushroom);
	if (texMushroom == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_FreeSurface(bmpMushroom);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

    int isStartGame = FALSE;
    int score = 0;
    int tempScore = 0;
    float timeElapsed = 0.0;

    char buf_game_title[18] = "Collect mushrooms";
    char buf_game_restart[24] = "Key R for start/restart";
    char buf_game_end[12] = "game over";
    char buf_score_text[10] = "";
    char buf_timer_text[10] = "";
    
    Mix_Chunk* gScore = Mix_LoadWAV( "resource/sound/pickupCoin.wav" );
    snprintf(buf_score_text, 10, "score:%d", score);
    snprintf(buf_timer_text, 10, "timer:%.2f", timeElapsed);

    //render text
    TTF_Font* Sans = TTF_OpenFont("resource/fonts/OpenSans-Light.ttf", 14); 
    SDL_Color White;
    White.r = 255;
    White.g = 255;
    White.b = 255;

    Uint32 startTime = 0;

    SDL_Surface* titleMessage = TTF_RenderText_Solid(Sans, buf_game_title, White);
    SDL_Surface* restartMessage = TTF_RenderText_Solid(Sans, buf_game_restart, White);
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, buf_score_text, White);
    SDL_Surface* timerMessage = TTF_RenderText_Solid(Sans, buf_timer_text, White);
    SDL_Surface* endGameMessage = TTF_RenderText_Solid(Sans, buf_game_end, White);
    
    SDL_Texture* MessageTitle = SDL_CreateTextureFromSurface(ren, titleMessage);
    SDL_Rect title_rect; //create a rect
    title_rect.x = 200;  //controls the rect's x coordinate 
    title_rect.y = 10; // controls the rect's y coordinte
    title_rect.w = 400; // controls the width of the rect
    title_rect.h = 100; // controls the height of the rect
    
    SDL_Texture* MessageRestart = SDL_CreateTextureFromSurface(ren, restartMessage);
    SDL_Rect Restart_rect; //create a rect
    Restart_rect.x = 100;  //controls the rect's x coordinate 
    Restart_rect.y = 200; // controls the rect's y coordinte
    Restart_rect.w = 400; // controls the width of the rect
    Restart_rect.h = 100; // controls the height of the rect

    SDL_Texture* MushroomMenu = SDL_CreateTextureFromSurface(ren, bmpMushroomMenu);
    SDL_Rect MushroomMenu_rect; //create a rect
    MushroomMenu_rect.x = 500;  //controls the rect's x coordinate 
    MushroomMenu_rect.y = 300; // controls the rect's y coordinte
    MushroomMenu_rect.w = 200; // controls the width of the rect
    MushroomMenu_rect.h = 300; // controls the height of the rect

    SDL_Texture* MessageTimer = SDL_CreateTextureFromSurface(ren, timerMessage);
    SDL_Rect timer_rect; //create a rect
    timer_rect.x = 10;  //controls the rect's x coordinate 
    timer_rect.y = 100; // controls the rect's y coordinte
    timer_rect.w = 200; // controls the width of the rect
    timer_rect.h = 100; // controls the height of the rect
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 10;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 200; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect
    
    SDL_Texture* MessageGameEnd = SDL_CreateTextureFromSurface(ren, endGameMessage);
    SDL_Rect Message_endGame_rect; //create a rect
    Message_endGame_rect.x = 0;  //controls the rect's x coordinate 
    Message_endGame_rect.y = 100; // controls the rect's y coordinte
    Message_endGame_rect.w = SCREEN_WIDTH; // controls the width of the rect
    Message_endGame_rect.h = 300; // controls the height of the rect

    //size game object
    int sizeFromMapWidth = 8;
    int sizeFromMapHeight = 5;

    SDL_Rect stretchRectMushroom;
    stretchRectMushroom.x = 0;
    stretchRectMushroom.y = 0;
    stretchRectMushroom.w = SCREEN_WIDTH / sizeFromMapWidth;
    stretchRectMushroom.h = SCREEN_HEIGHT / sizeFromMapHeight;

    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = SCREEN_WIDTH / sizeFromMapWidth;
    stretchRect.h = SCREEN_HEIGHT / sizeFromMapHeight;
    
    int speed = 50;
    int speedYTop = speed;
    int speedYBottom = speed;
    int speedXLeft = speed;
    int speedXRight = speed;
    int bordersMapWidth = sizeFromMapWidth * speed * 2;
    int bordersMapHeight = sizeFromMapHeight * speed * 2;
    //int possiblyAmoutMushrooms = sizeFromMapWidth * (sizeFromMapHeight + 1); 
    //printf("borders width = %u \n", bordersMapWidth);
    //printf("borders height = %u \n", bordersMapHeight);
    
    //int amoutTotalCells = 64;

    struct mushroom mushrooms[CELLS];    
    int isExistsMush[CELLS] = {FALSE};
    int amountExist = 0;
    for (int i = 0; i < CELLS; ++i){
        int r = genRandoms(0,5);
        mushrooms[i].IsExist = FALSE;
        if (r == 0){
            //printf("num random mushrooms = %u \n", r); 
            isExistsMush[i] = TRUE;
            mushrooms[i].IsExist = TRUE;
            amountExist++;
        }
    }
    //printf("Amount mushrooms = %u \n", CELLS);
    //printf("Amount mushrooms need to eat = %u \n", amountExist);
	SDL_FreeSurface(bmpHero);

    //Main loop flag
    int quit = FALSE;

    //Event handler
    SDL_Event e;
    
    int x = 0;
    int y = 0;
    for (int i = 0; i < CELLS; ++i){
            mushrooms[i].RectMushroom.x = (x * speed) * 2;
            mushrooms[i].RectMushroom.y = (y * speed) * 2;
            mushrooms[i].RectMushroom.w = SCREEN_WIDTH / sizeFromMapWidth;
            mushrooms[i].RectMushroom.h = SCREEN_HEIGHT / sizeFromMapHeight;
            x++;
            //printf("x = %u \n", x);
            if (x == 8){
                x = 0;
                y++;
            }
            if (y == 5){
                y = 0;
            }
            //printf("y = %u \n", y);
    }

    startTime = SDL_GetTicks();
    int isExistTimer = TRUE;
    //game loop
    while( !quit ){

        if (isExistTimer && isStartGame){
            snprintf(buf_timer_text, 10, "timer:%.2f", (SDL_GetTicks() - startTime) * 0.001);
            timerMessage = TTF_RenderText_Solid(Sans, buf_timer_text, White);
            MessageTimer = SDL_CreateTextureFromSurface(ren, timerMessage);
            //printf("time: %0.2f \n", (SDL_GetTicks() - startTime) * 0.001);
        }
        SDL_RenderClear(ren);
        if (isStartGame){
		    SDL_RenderCopy(ren, tex, NULL, NULL);
            for (int i = 0; i < CELLS; ++i){
                if (mushrooms[i].IsExist){ 
                    SDL_RenderCopyEx(ren, texMushroom, NULL, &mushrooms[i].RectMushroom, 0, NULL, SDL_FLIP_NONE);
                }
            }



            SDL_RenderCopyEx(ren, texHero, NULL, &stretchRect, 0, NULL, SDL_FLIP_NONE);
            if (((SDL_GetTicks() - startTime) * 0.001) > timer){
                isExistTimer = FALSE;
                speed = 0;
                //startTime = 0;
                SDL_RenderCopyEx(ren, MessageGameEnd, NULL, &Message_endGame_rect, 0, NULL, SDL_FLIP_NONE); 
            }
            SDL_RenderCopy(ren, MessageTimer, NULL, &timer_rect);
            SDL_RenderCopy(ren, Message, NULL, &Message_rect);
        }else{
            SDL_RenderCopy(ren, MessageTitle, NULL, &title_rect);
            SDL_RenderCopy(ren, MessageRestart, NULL, &Restart_rect);
            SDL_RenderCopy(ren, MushroomMenu, NULL, &MushroomMenu_rect);
        }
		SDL_RenderPresent(ren);


        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 ){
            //User requests quit
            if( e.type == SDL_QUIT ){
                quit = TRUE;
            }

            //If a key was pressed
              if( e.type == SDL_KEYDOWN ){
                 
                for (int i = 0; i < CELLS; ++i){
                   
                    if (checkCollision(mushrooms[i].RectMushroom, stretchRect) 
                            && mushrooms[i].IsExist
                            && isStartGame){
                        score += 1;
                        startTime = SDL_GetTicks();

                        if (score == amountExist + tempScore){
                            tempScore += amountExist; 
                            amountExist = 0;
                                for (int i = 0; i < CELLS; ++i){
                                    int r = genRandoms(0,5);
                                    mushrooms[i].IsExist = FALSE;
                                    if (r == 0){
                                        //printf("num random mushrooms = %u \n", r); 
                                        isExistsMush[i] = TRUE;
                                        mushrooms[i].IsExist = TRUE;
                                        ++amountExist;
                                    }
                                }
                            //printf("score = %u \n", score);
                            //printf("amountExist = %u \n", amountExist);
                            //printf("Collect all mushrooms");
                        }
                        snprintf(buf_score_text, 10, "score:%d", score);
                        //printf("score = %u \n", score);



                        surfaceMessage = TTF_RenderText_Solid(Sans, buf_score_text, White);
                        Message = SDL_CreateTextureFromSurface(ren, surfaceMessage);
                        mushrooms[i].IsExist = FALSE;
                        Mix_PlayChannel( -1, gScore, 1);
                    }

                }

                    // ------------------------------------------                    
                    //  Constraint
                    // ------------------------------------------
                    if (stretchRect.y <= 0){
                        speedYTop = 0;
                    }else{
                        speedYTop = speed;
                    }

                    if (stretchRect.y >= bordersMapHeight){
                        speedYBottom = 0;
                    }else{
                        speedYBottom = speed;
                    }

                    if (stretchRect.x <= 0){
                        speedXLeft = 0;
                    }else{
                        speedXLeft = speed;
                    }

                    if (stretchRect.x >= bordersMapWidth - speed * 2){
                        speedXRight = 0;
                    }else{
                        speedXRight = speed;
                    }
                    // ------------------------------------------
                    switch( e.key.keysym.sym ){
                        case SDLK_UP:
                            //printf("x hero = %u \n", stretchRect.x); 
                            //printf("y hero = %u \n", stretchRect.y);
                            stretchRect.y -= speedYTop; 
                            break;
                        case SDLK_DOWN:
                            //printf("x hero = %u \n", stretchRect.x); 
                            //printf("y hero = %u \n", stretchRect.y);
                            stretchRect.y += speedYBottom; 
                            break;
                        case SDLK_LEFT:
                            //printf("x hero = %u \n", stretchRect.x); 
                            //printf("y hero = %u \n", stretchRect.y);
                            stretchRect.x -= speedXLeft; 
                            break;
                        case SDLK_RIGHT:
                            //printf("x hero = %u \n", stretchRect.x); 
                            //printf("y hero = %u \n", stretchRect.y);
                            stretchRect.x += speedXRight; 
                            break;
                        case SDLK_r:
                            isStartGame = TRUE;
                            isExistTimer = TRUE;
                            speed = 50;
                            startTime = SDL_GetTicks();
                            tempScore = 0; 
                            score = 0;
                            amountExist = 0;
                                for (int i = 0; i < CELLS; ++i){
                                    int r = genRandoms(0,5);
                                    mushrooms[i].IsExist = FALSE;
                                    if (r == 0){
                                        //printf("num random mushrooms = %u \n", r); 
                                        isExistsMush[i] = TRUE;
                                        mushrooms[i].IsExist = TRUE;
                                        ++amountExist;
                                    }
                                }
                            break;
                        case SDLK_ESCAPE:
                            quit = TRUE;
                            break;
                    }
                    
                }


            }

        }

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
