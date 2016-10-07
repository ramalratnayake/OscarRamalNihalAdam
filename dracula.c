// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include <time.h>

int isInTrail (DracView gameState, LocationID move);
int isValidMove(DracView gameState, LocationID move);
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
        
        while (isValidMove(gameState, move) == FALSE){
            //i = rand()%a;
            i++; 
		    move = locs[i];
                   
        }
        printf ("my move is %s\n", idToName(move));
        registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
        

/*		// checks for hunter locations to make a better decision
		int b;
        int *hunterLocs = whereCanTheyGo(gameState,&b,PLAYER_LORD_GODALMING, 1,1,1);
		
		while (here(hunterLocs,move,b) == 0) {
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
    
int isValidMove(DracView gameState, LocationID move){
    if(isInTrail (gameState, move) == TRUE){
        return FALSE;
    }else if (move == ST_JOSEPH_AND_ST_MARYS){
        return FALSE;
    }else if (move == whereIs(gameState, PLAYER_DRACULA)){
        return FALSE;
    }
    return TRUE;
}
            
