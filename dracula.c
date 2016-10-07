// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include <time.h>

int isInTrail (DracView gameState, LocationID move);
/*
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
*/

void decideDraculaMove(DracView gameState)
{
	// TODO ...
	// Replace the line below by something better

	if(giveMeTheRound(gameState) == 0){
		registerBestPlay("CD","Mwuhahahaha");
    } else {
		int a;
        int *locs = whereCanIgo(gameState, &a, 1,1);
        int q = 0;
        while (q<a){
            printf ("%s, ", idToName(locs[q]));
            q++;
        }
        printf("\n");
		srand(time(NULL));
		// dumb move intially
        int i = 1;
        int move = locs[i];
        
        while ((isInTrail (gameState, move) == TRUE || move == ST_JOSEPH_AND_ST_MARYS || move == CASTLE_DRACULA) && i<a){
		    move = locs[i];
            i++;            
        }
        printf ("my move is %s\n", idToName(move));
        if(i != a){
            registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
        }
/*
		// checks for hunter locations to make a better decision
		int b;
        int *hunterLocs = whereCanTheyGo(gameState,&b,PLAYER_LORD_GODALMING, 1,1,1);
		
		while (here(hunterLocs,move,b) == 1) {
			move = locs[ rand()%a ];
		}

		registerBestPlay(idToName(move),"Mwuhahahaha");
*/
	}
}

int isInTrail (DracView gameState, LocationID move){
    LocationID trail [TRAIL_SIZE];
    giveMeTheTrail(gameState,PLAYER_DRACULA,trail);
    int  i = 0;
    while (i< TRAIL_SIZE){
        if (move == trail[i]){
            return TRUE;
        }
        i++;
    }
    return FALSE;
}
            
