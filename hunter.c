// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

// A Good Dracula Pun
// by Ramal, Oscar, Adam, Nihal 

/*

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Notes: 
   - Random movement causes dq's; due to rail/boat connection ~~~ Fixed
     using jas's connected locations function
   - Random movement causes dq's; unknown reason -> Fixed
     using jas's views
   - Inefficiences exist 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Current Strategy:
   - randomly patrolling
   - if a hunter encounters dracs trail, then follow that trail
   - if drac's current location is known then predict the move and take
     take that move
   - if drac's curr loc is known the take the shortest path
   - else take shortest path to trail
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Strategy ideas:
   - rank locations based of connected locations / probability
   - backTrace, determine general location of drac based off past plays
   - determine location of vampires and destroy them when they appear
   - compare functions

   - using a list, and knocking out locations of where drac can't be
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "Game.h"
#include "HunterView.h"

#define MAX_ITERATIONS 100

static int makeRandomMove(HunterView hv,int player,int ver) {

   int a;
   int *locs = whereCanIgo(hv, &a, 1,1,1);
   
   int r = rand()%a;
   
   if (ver == 1) {
      return locs[a-r];
   } else if (ver == 2) {
      return locs[a - r*(giveMeTheRound(hv)%2)];
   } else {
      return locs[r];
   }
}

static int notHere(int *locs, int x, int size) {
   int i = 0;
   while(i < size) {
      if(locs[i] == x) {
         // 0 -> found         
         return 0;
      }
      i++;
   }
   // 1 -> not found
   return 1;
}



/*
// compares the trails to all possible positions to move to 
static int checkDracTrail(HunterView hv,int player) {
   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,PLAYER_DRACULA,t);
   int a;
   int *locs = whereCanTheyGo(hv,&a,player,1,1,1);
   
   int x = 0;

   while(x < a) {
      if (notHere(t,locs[x],TRAIL_SIZE) == 0) {
         return locs[x];
      }
      x++;
   }
   return -1;
}
*/

static int checkTrail(HunterView hv, int player,int move,int version) {
   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,player,t);
   
   if (version == 1) {

      if (notHere(t,move,TRAIL_SIZE) == 1) {
         return 0;
      } else {
         return 1;
      } 
   } else {
      // checks prev 3 moves
      if (move == t[0] || move == t[1]) {
         return 1;
      } else {
         return 0;
      }
   }   
}

static int checkForOthers(HunterView hv, int player) {
   int g = whereIs(hv,PLAYER_LORD_GODALMING);
   int s = whereIs(hv,PLAYER_DR_SEWARD); 
   int v = whereIs(hv,PLAYER_VAN_HELSING); 
   int m = whereIs(hv,PLAYER_MINA_HARKER);

   int p = whereIs(hv,player);

   if(player ==  PLAYER_LORD_GODALMING){
      if(p == s || p == v || p == m) {
         return 0;
      } else {
         return 1;
      }
   } else if(player ==  PLAYER_DR_SEWARD){
      if(p == g || p == v || p == m) {
         return 0;
      } else {
         return 1;
      }
   } else if(player ==  PLAYER_VAN_HELSING){
      if(p == s || p == g || p == m) {
         return 0;
      } else {
         return 1;
      }
   } else if(player ==  PLAYER_MINA_HARKER){
      if(p == s || p == v || p == g) {
         return 0;
      } else {
         return 1;
      }
   }
   return -1;
}

static int predictingDracLocs(HunterView hv) {
   int n;
   int *dracLocs = whereCanTheyGo(hv,&n,PLAYER_DRACULA,1,0,1);

   int a;
   int *hunterLocs = whereCanIgo(hv, &a, 1,1,1);

   int dracCurrLoc = whereIs(hv,PLAYER_DRACULA);

   if(validPlace(dracCurrLoc) != TRUE) {
      return -1;
   } else {
      int x = 0;
      while(x < a) {
         if (notHere(dracLocs,hunterLocs[x],n) == 0) {
            return hunterLocs[x];
         }
         x++;
      }
      return -1;
   }
}

static int inferDracLocs(HunterView hv,int from) {
   int n;

   int *dracLocs = locsNoPlayer(hv,&n,from,1,0,1);

   int a;
   int *hunterLocs = whereCanIgo(hv, &a, 1,1,1);

   int x = 0;
   while(x < a) {
      if (notHere(dracLocs,hunterLocs[x],n) == 0) {
         return hunterLocs[x];
      }
      x++;
   }
   return -1;
}


/*
static int checkTwoArray(int *one, int oneL, int *two, int twoL) {
   int x = 0;
   int y = 0;
   while(y < oneL) {
      x = 0;
      while(x < twoL) {
         if(one[y] == two[x] ){
            return two[x];
         }
         x++;
      }
      y++;
   }
   return -1;
}*/

static void showArray(int *a, int l) {
   int i = 0;
   while(i < l) {
      if(validPlace(a[i]) == TRUE) {
         printf("%s, ",idToName(a[i]));
      } else {
         printf("%d, ",a[i]);   
      }
      i++;
   }
   printf("\n");
}

static int giveDracTrail(HunterView hv) {
   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,PLAYER_DRACULA,t);
   int result = -1;
   int x = TRAIL_SIZE-1;

   showArray(t,TRAIL_SIZE);

   while(x >= 0) {
      if ( validPlace(t[x]) == TRUE ) {
         result = t[x];
      }
      x--;
   }
   return result;
}

void decideHunterMove(HunterView gameState)
{
   srand(time(NULL));
   int player = whoAmI(gameState);
   int currLoc = whereIs(gameState,player);
   int round = giveMeTheRound(gameState);

   if(currLoc == NOWHERE ){
        if(player ==  PLAYER_LORD_GODALMING){
            registerBestPlay(idToAbbrev(EDINBURGH),"Starting hunt in Edinburgh...");
        } else if(player ==  PLAYER_DR_SEWARD){
            registerBestPlay(idToAbbrev(CADIZ),    "Starting hunt in Cadiz...");
        } else if(player ==  PLAYER_VAN_HELSING){
            registerBestPlay(idToAbbrev(SZEGED),   "Starting hunt in Szeged...");
        } else if(player ==  PLAYER_MINA_HARKER){
            registerBestPlay(idToAbbrev(BARI),     "Starting hunt in Bari...");
        }
   } else {
      if (howHealthyIs(gameState, player ) < 4 ) {
        registerBestPlay(idToAbbrev(currLoc) ,"Hurt badly, resting...");
      } else if (round%6 == 0) {
         registerBestPlay(idToAbbrev(currLoc) , "Researching...");
      } else {
         int move = -1;
         int a;
         int *locs = whereCanIgo(gameState, &a, 1,1,1);
         int dracCurrLoc = whereIs(gameState,PLAYER_DRACULA);

         if(validPlace(dracCurrLoc) == TRUE) {
            //============================================================
            // Dracula location is known -> goes towards it
            //============================================================
            if(currLoc == dracCurrLoc) {
               int predict = predictingDracLocs(gameState);
               if(predict != -1) {
                  move = predict;
                  registerBestPlay(idToAbbrev(move),"Predicting...");
               }
            } else {
               int length;               
               int *p = minPathFinder(gameState, currLoc, dracCurrLoc, &length);
               
               if (notHere(locs,p[1],a) == 0) {
                  move = p[1];
                  registerBestPlay(idToAbbrev(move),"Hunting Drac's trail using the shortest path I know!");
               }
            }
         } else {
            //============================================================
            // Dracula location is unknown -> goes towards valid trail loc
            //============================================================
            
            int dracT = giveDracTrail(gameState);
            
            if ( validPlace(dracT) == TRUE ){
               printf("Drac's location is unknown using trail: %s\n", idToName(dracT)); 
               if(currLoc == dracT) {
                  int infer = inferDracLocs(gameState,dracT);
                  if(infer != -1) {
                     move = infer;
                     registerBestPlay(idToAbbrev(move),"Infering...");
                  }
               } else {
                  int length;               
                  int *p = minPathFinder(gameState, currLoc, dracT, &length);
                  if (notHere(locs,p[1],a) == 0) {
                     move = p[1];
                     registerBestPlay(idToAbbrev(move),"Hunting Drac's trail using the shortest path I know!");
                  }
               }
            } else {
               // apply more vigorious scan of the pastplays
               int *back = backtraceDrac(gameState);
              
               int i = round;
               int check;
               while(i >= 0) {
                  if(validPlace(back[i]) == TRUE) {
                     check = back[i];
                     break;
                  }
                  i--;
               }
               
               if(currLoc == check) {
                  int infer = inferDracLocs(gameState,check);
                  if(infer != -1) {
                     move = infer;
                     registerBestPlay(idToAbbrev(move),"Infering...");
                  }
               } else {
                  int length;               
                  int *p = minPathFinder(gameState, currLoc, check, &length);
                  if (notHere(locs,p[1],a) == 0) {
                     move = p[1];
                     registerBestPlay(idToAbbrev(move),"Hunting Drac's trail using the shortest path I know!");
                  }
               }
            }
            
            if (validPlace(move) != TRUE) {
               
               move = makeRandomMove(gameState,player,0);
               int i = 0;
               while(i <= MAX_ITERATIONS) { 
                  if(checkTrail(gameState,player,move,1) == 0 && checkForOthers(gameState,player) == 1 ) {
                     break;
                  }
                  move = makeRandomMove(gameState,player,0);
                  i++;
               }
               printf("Otherwise unknown (this should never happen): %s\n", idToName(move));               
               registerBestPlay(idToAbbrev(move),"Patrolling..."); 
            }
         }
        
      }
   }
}