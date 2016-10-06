// dracula.c

// Implementation of your "Fury of Dracula" Dracula AI



#include <stdlib.h>

#include <stdio.h>

#include "Game.h"

#include "DracView.h"

#include <time.h>



static int here(int *locs, int x, int size) {

   int i = 0;

   while(i < size) {

      if(locs[i] == x) {

         return 1;

      }

      i++;

   }

   return 0;

}



void decideDraculaMove(DracView gameState)

{

	// TODO ...

	// Replace the line below by something better



	if(giveMeTheRound(gameState) == 0){

		registerBestPlay("CD","Mwuhahahaha");

    } else {

		

		int a;

        int *locs = whereCanIgo(gameState, &a, 1,1);



		srand(time(NULL));

		// dumb move intially

		int move = locs[ rand()%a ];

		registerBestPlay(idToAbbrev(move),"Mwuhahahaha");



		// checks for hunter locations to make a better decision

		int b;

        int *hunterLocs = whereCanTheyGo(gameState,&b,PLAYER_LORD_GODALMING, 1,1,1);

		

		while (here(hunterLocs,move,b) == 1) {

			move = locs[ rand()%a ];

		}



		registerBestPlay(idToAbbrev(move),"Mwuhahahaha");

	}

}

