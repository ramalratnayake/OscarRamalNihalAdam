// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
#include <assert.h>

#define MAX_ITERATIONS 100

/*
////////////////////////////////////////////
Notes: 
   - when transported to JM needs to go back to patrolling region
   - 

////////////////////////////////////////////
*/

/*
Strategy:
   - randomly patrolling a region

*/

/*
Strategy ideas:
   - 
*/

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
static int makeSemiRandomMove(HunterView hv,int player) {

   int a;
   int *locs = whereCanIgo(hv, &a, 1,1,1);
   
   int round = giveMeTheRound(hv);
   int i;
   
   if (round%3 == 0) {
      i = locs[a];
   } else if (round%3 == 1) {
      i = locs[a/2];
   } else {
      i = locs[3/4*a];
   }

   return i;
}*/

static int isLegal(HunterView hv, int move) {
   int result = TRUE;

   if(idToType(move) == SEA) {
      result = FALSE;
   }
   return result;
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
      if (move == t[0] || move == t[1] || move == t[2]) {
         return 1;
      } else {
         return 0;
      }
   }   
}

// compares the trails to all possible positions to move to 
static int checkDracTrail(HunterView hv, int move) {
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

void decideHunterMove(HunterView gameState)
{
   srand(time(NULL));
   int player = whoAmI(gameState);
   int round = giveMeTheRound(gameState);


   if(round == 0){
        if(player ==  PLAYER_LORD_GODALMING){
            registerBestPlay(idToAbbrev(MANCHESTER),"I'm on holiday in Manchester");
        } else if(player ==  PLAYER_DR_SEWARD){
            registerBestPlay(idToAbbrev(ROME),"I'm on holiday in Rome");
        } else if(player ==  PLAYER_VAN_HELSING){
            registerBestPlay(idToAbbrev(MADRID),"I'm on holiday in Madrid");
        } else if(player ==  PLAYER_MINA_HARKER){
            registerBestPlay(idToAbbrev(SZEGED),"I'm on holiday in Szeged");
        } 
   } else {
         if ( whereIs(gameState,player) == ST_JOSEPH_AND_ST_MARYS && howHealthyIs(gameState,player) < 9 ) {
            registerBestPlay("JM", "Resting at JM");
         } else if ( howHealthyIs(gameState,player) <= 6 ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,player)),"Resting..."); 
         } else {

            int move = makeRandomMove(gameState,player,0);

            while (move == whereIs(gameState,player)) {
               move = makeRandomMove(gameState,player,1);
            }

            int x = 0;
            while (isLegal(gameState,move) == FALSE && checkTrail(gameState,player,move,1) == TRUE ) {
               move = makeRandomMove(gameState,player,2);
               if (x > MAX_ITERATIONS) {
                  break;
               }
               x++;
            }

            // Picks in move to actively hunts dracula, if within dracs trail     
            int check = checkDracTrail(gameState,move);            
            if (check != -1) {
               move = check;                                  
            }

            registerBestPlay(idToAbbrev(move),"Patrolling...");
         }

         /* else if (giveMeTheRound(gameState)%6 == 0 ||  howHealthyIs(gameState, PLAYER_LORD_GODALMING ) < 3 ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_LORD_GODALMING)) ,"Resting...");
         } else {
			   while( notHere(north,move,19) == 1) {
				   move = locs[ rand()%a ];
				   registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			   }
         }*/
   }
}

/*


   // set up regions
	int east[19];
	east[0]=VIENNA;
	east[1]=ZAGREB;
	east[2]=BUDAPEST;
	east[3]=SZEGED;
	east[4]=ST_JOSEPH_AND_ST_MARYS;
	east[5]=SARAJEVO;
	east[6]=BELGRADE;
	east[7]=KLAUSENBURG;
	east[8]=CASTLE_DRACULA;
	east[9]=GALATZ;
	east[10]=BUCHAREST;
	east[11]=SOFIA;
	east[12]=VALONA;
	east[13]=ATHENS;
	east[14]=SALONICA;
	east[15]=CONSTANTA;
	east[16]=BLACK_SEA;
	east[17]=IONIAN_SEA;
	east[18]=VARNA;

	int north[17];
	north[0]=EDINBURGH;
	north[1]=MANCHESTER;
	north[2]=LONDON;
	north[3]=SWANSEA;
	north[4]=PLYMOUTH;
	north[5]=LIVERPOOL;
	north[6]=DUBLIN;
	north[7]=GALWAY;
	north[8]=IRISH_SEA;
	north[9]=ENGLISH_CHANNEL;
	north[10]=NORTH_SEA;
	north[11]=LE_HAVRE;
	north[12]=PARIS;
	north[13]=HAMBURG;
	north[14]=AMSTERDAM;
	north[15]=BRUSSELS;
	north[16]=COLOGNE;

	int central[15];
	central[0]=BERLIN;
	central[1]=LEIPZIG;
	central[2]=PRAGUE;
	central[3]=NUREMBURG;
	central[4]=FRANKFURT;
	central[5]=STRASBOURG;
	central[6]=ZURICH;
	central[7]=MUNICH;
	central[8]=VENICE;
	central[9]=ROME;
	central[10]=NAPLES;
	central[11]=BARI;
	central[12]=TYRRHENIAN_SEA;
	central[13]=ADRIATIC_SEA;
	central[14]=FLORENCE;

	int south[19];
	south[0]=BORDEAUX;
	south[1]=CLERMONT_FERRAND;
	south[2]=GENEVA;
	south[3]=MILAN;
	south[4]=GENOA;
	south[5]=MARSEILLES;
	south[6]=CAGLIARI;
	south[7]=TOULOUSE;
	south[8]=BARCELONA;
	south[9]=ALICANTE;
	south[10]=GRANADA;
	south[11]=LISBON;
	south[12]=BAY_OF_BISCAY;
	south[13]=MEDITERRANEAN_SEA;
	south[14]=MADRID;
	south[15]=SANTANDER;
	south[16]=CADIZ;
	south[17]=SARAGOSSA;
	south[18]=NANTES;










printf("Im here: %s\n", idToAbbrev(whereIs(gameState,PLAYER_LORD_GODALMING)) );
   
         int i = 0;
         while( i < a) {
         	printf("%s, ", idToAbbrev(locs[i]) );
         	i++;
         }
         printf("\n");




		} else if ( whoAmI(gameState) == PLAYER_DR_SEWARD) {
      
         
         move = makeRandomMove(gameState);

         if(move == whereIs(gameState,player)) {
            move = makeRandomMove(gameState);
         }

		   registerBestPlay(idToAbbrev(move),"Patrolling...");


		   
         if ( whereIs(gameState,PLAYER_DR_SEWARD) == ST_JOSEPH_AND_ST_MARYS ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_DR_SEWARD)),"Resting at JM"); 
         } else if (giveMeTheRound(gameState)%6 == 0 ||  howHealthyIs(gameState, PLAYER_DR_SEWARD ) < 3 ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_DR_SEWARD)) ,"Resting...");
         } else {
			   while( notHere(central,move,19) == 1) {
				   move = locs[ rand()%a ];
				   registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			   }
         }
		} else if ( whoAmI(gameState) == PLAYER_VAN_HELSING) {
         

         move = makeRandomMove(gameState);

         if(move == whereIs(gameState,player)) {
            move = makeRandomMove(gameState);
         }

		   registerBestPlay(idToAbbrev(move),"Patrolling...");
  


		   
         if ( whereIs(gameState,PLAYER_VAN_HELSING) == ST_JOSEPH_AND_ST_MARYS ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_VAN_HELSING)),"Resting at JM"); 
         } else if (giveMeTheRound(gameState)%6 == 0 ||  howHealthyIs(gameState, PLAYER_VAN_HELSING ) < 3 ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_VAN_HELSING)) ,"Resting...");
         } else {
			   while( notHere(south,move,19) == 1) {
				   move = locs[ rand()%a ];
				   registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			   }
         }
		} else if ( whoAmI(gameState) == PLAYER_MINA_HARKER) {
        
            
         move = makeRandomMove(gameState);

         if(move == whereIs(gameState,player)) {
            move = makeRandomMove(gameState);
         }
      
		   registerBestPlay(idToAbbrev(move),"Patrolling...");

		   
         // east has Jm located in it 
         if (giveMeTheRound(gameState)%6 == 0 ||  howHealthyIs(gameState, PLAYER_MINA_HARKER ) < 3 ) {
            registerBestPlay(idToAbbrev(whereIs(gameState,PLAYER_MINA_HARKER)) ,"Resting...");
         } else {
			   while( notHere(east,move,19) == 1) {
				   move = locs[ rand()%a ];
				   registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			   }
         }
      }

		//registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
	}

*/