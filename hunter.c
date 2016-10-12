// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

// A Good Dracula Pun
// by Ramal, Oscar, Adam, Nihal 

/*

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Notes: 
   - Random movement causes dq's; due to rail/boat connection ~~~ Fixed
     using jas's connected locations function
   - Random movement causes dq's; unknown reason -> possibly due to health

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Current Strategy:
   - randomly patrolling
   - if a hunter encounters dracs trail, then follow that trail
   - if drac's current location is known then predict the move and take
     take that move
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Strategy ideas:
   - have a hunter alert others if drac is spotted 
   - have hunts move towards that position
   - Needs to strategically rest, eg every 6 rounds rest to do research
   - rank locations based of connected locations
   - Shortest path function
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
#define DRAC_FOUND 0


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


/*
static int isLegal(HunterView hv, int move) {
   int result = FALSE;

   if(idToType(move) == SEA) {
      result = FALSE;
   } else {
      result = TRUE;
   }

   return result;
}

static int randomLocation(void) {
   int i = rand()%MAX_MAP_LOCATION;
   return i;
}*/

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

// compares the trails to all possible positions to move to 
static int checkDracTrail(HunterView hv) {
   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,PLAYER_DRACULA,t);

   int a;
   int *locs = whereCanIgo(hv, &a, 1,1,1);

   int x = 0;

   while(x < a) {
      if (notHere(t,locs[x],TRAIL_SIZE) == 0) {
         return locs[x];
      }
      x++;
   }
   return -1;
}

// compares all possible positions drac can move to, 
// then returns loc that matches with the hunter

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

static void showArray(int *a, int l) {
   int i = 0;
   while(i < l) {
      printf("%s, ",idToName(a[i]));
      i++;
   }
   printf("\n");
}

// 1 for all clear
// 0 for too close
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
/*
static char *createMessage(int check, int move) {
      
   if (check == DRAC_FOUND) {
      char *m = "! Drac trail found, move to ";
      m[strlen(m)-1] = *(idToName(move));
      return m;
   }
   return NULL;
}*/

void decideHunterMove(HunterView gameState)
{
   srand(time(NULL));
   int player = whoAmI(gameState);
   int currLoc = whereIs(gameState,player);
   int round = giveMeTheRound(gameState);

   if(currLoc == NOWHERE ){
        if(player ==  PLAYER_LORD_GODALMING){
            registerBestPlay(idToAbbrev(EDINBURGH),"Starting hunt in EDINBURGH......................LG");
        } else if(player ==  PLAYER_DR_SEWARD){
            registerBestPlay(idToAbbrev(CADIZ),    "Starting hunt in CADIZ............................");
        } else if(player ==  PLAYER_VAN_HELSING){
            registerBestPlay(idToAbbrev(SZEGED),   "Starting hunt in SZEGED...........................");
        } else if(player ==  PLAYER_MINA_HARKER){
            registerBestPlay(idToAbbrev(BARI),     "Starting hunt in BARI.............................");
        }
   } else {
      if (howHealthyIs(gameState, player ) < 4 ) {
        registerBestPlay(idToAbbrev(currLoc) ,     "Hurt badly, resting...............................");
      } else if (round%6 == 0) {
         registerBestPlay(idToAbbrev(currLoc) ,    "Researching.......................................");
      } else {
         int move = makeRandomMove(gameState,player,0);

         printf("Health: %d\n",howHealthyIs(gameState, player ));

         printf("Moving from: %s\n", idToName(currLoc));

         printf("Initial move: %s\n", idToName(move));

         int a;
         int *locs = whereCanIgo(gameState, &a, 1,1,1);

         showArray(locs,a);
               
         int i = 0;
         while(i <= MAX_ITERATIONS) { 
            if(checkTrail(gameState,player,move,1) == 0 && checkForOthers(gameState,player) == 1 ) {
               break;
            }
            move = makeRandomMove(gameState,player,0);
            i++;
         }
         
         registerBestPlay(idToAbbrev(move),"Patrolling........................................");

         printf("Decision after considering connections: %s\n", idToName(move));

         int dracCurrLoc = whereIs(gameState,PLAYER_DRACULA);
         if( validPlace(dracCurrLoc) != TRUE  ) {
            // Picks a move to actively hunts dracula, if within dracs trail     
            int check = checkDracTrail(gameState);            
            if (check != -1 ) {
               move = check;
               
               if(player ==  PLAYER_LORD_GODALMING){
                  registerBestPlay( idToAbbrev(move) ,"Found Drac's Trail, follow me!..................LG");
               } else if(player ==  PLAYER_DR_SEWARD){
                  registerBestPlay( idToAbbrev(move) ,"Found Drac's Trail, follow me!..................LS");
               } else if(player ==  PLAYER_VAN_HELSING){
                  registerBestPlay( idToAbbrev(move) ,"Found Drac's Trail, follow me!..................LV");
               } else if(player ==  PLAYER_MINA_HARKER){
                  registerBestPlay( idToAbbrev(move) ,"Found Drac's Trail, follow me!..................LM");
               }
            }
         } else {
            // Picks a move to actively hunts dracula, if within dracs predicted location
            
            int predict = predictingDracLocs(gameState);
            if (predict != -1 ) {
               move = predict;                                
               registerBestPlay(idToAbbrev(move),"Hunting Drac using prediction.....................");
            }
         }
         printf("Final move (using basic strats): %s\n", idToName(move));
      
         // implementing more complex strategy (backtrace)


         if( validPlace(dracCurrLoc) == TRUE ) {
            int length;
            int *p = minPathFinder(gameState, player, dracCurrLoc, &length);
            
            if (notHere(locs,p[1],length) == 0) {
               move = p[1];
               registerBestPlay(idToAbbrev(move),"Hunting Drac using the shortest path I know!......");
            }
         } /*else {
            // scan messages
            // note: do this 4 times
            char *m = giveMeTheMessage(gameState);
            int l = strlen(m) -1;
            int leader = PLAYER_LORD_GODALMING;
            
            int count = 1;
         
            while( count < 4) {
               if(m[l-count*50 -1] == 'L') {
                  if(m[l-count*50] == 'G') {
                     leader = PLAYER_LORD_GODALMING;
                  } else if (m[l-count*50] == 'S') {
                     leader = PLAYER_DR_SEWARD;
                  } else if (m[l-count*50] == 'V')  {
                     leader = PLAYER_VAN_HELSING;
                  } else if (m[l-count*50] == 'M')  {
                     leader = PLAYER_MINA_HARKER;
                  }
               }
               count++;            
            }
            int ln;
            int *p = minPathFinder(gameState, player, whereIs(gameState,leader), &ln);
            
            if (notHere(locs,p[1],ln) == 0) {
               move = p[1];
               registerBestPlay(idToAbbrev(move),"Hunting Drac trail using the shortest path I know!");
            }
         }*/

         printf("Final move (using more advanced strats): %s\n", idToName(move));
         
      }
   }
}