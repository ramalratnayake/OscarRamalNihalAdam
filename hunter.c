// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>

static int notHere(int *locs, int x, int size) {
   int i = 0;
   while(i < size) {
      if(locs[i] == x) {
         return 0;
      }
      i++;
   }
   return 1;
}

void decideHunterMove(HunterView gameState)
{
    // TODO ...
    // Replace the line below by something better

	if(giveMeTheRound(gameState) == 0){
        if(whoAmI(gameState) ==  PLAYER_LORD_GODALMING){
            registerBestPlay(idToName(MANCHESTER),"I'm on holiday in Geneva");
        } else if(whoAmI(gameState) ==  PLAYER_DR_SEWARD){
            registerBestPlay(idToName(PARIS),"I'm on holiday in Geneva");
        } else if(whoAmI(gameState) ==  PLAYER_VAN_HELSING){
            registerBestPlay(idToName(MADRID),"I'm on holiday in Geneva");
        } else if(whoAmI(gameState) ==  PLAYER_MINA_HARKER){
            registerBestPlay(idToName(SZEGED),"I'm on holiday in Geneva");
        } 
    } else {
		
		int a;
        int *locs = whereCanIgo(gameState, &a, 1,1,1);

		srand(time(NULL));

		int move = locs[ rand()%a ];

		while( notHere(int *regionArray, int move, int size) == 0) {
			move = locs[ rand()%a ];
		}

		registerBestPlay(idToName(move),"I'm on holiday in Geneva");
	}
}