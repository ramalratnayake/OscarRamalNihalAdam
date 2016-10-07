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

	if(giveMeTheRound(gameState) == 0){
        if(whoAmI(gameState) ==  PLAYER_LORD_GODALMING){
            registerBestPlay(idToAbbrev(MANCHESTER),"I'm on holiday in Manchester");
        } else if(whoAmI(gameState) ==  PLAYER_DR_SEWARD){
            registerBestPlay(idToAbbrev(ROME),"I'm on holiday in Rome");
        } else if(whoAmI(gameState) ==  PLAYER_VAN_HELSING){
            registerBestPlay(idToAbbrev(MADRID),"I'm on holiday in Madrid");
        } else if(whoAmI(gameState) ==  PLAYER_MINA_HARKER){
            registerBestPlay(idToAbbrev(SZEGED),"I'm on holiday in Szeged");
        } 
    } else {
		
		int a;
        int *locs = whereCanIgo(gameState, &a, 1,1,1);

		srand(time(NULL));


		// note always update in case we run out of time
		int move = locs[ rand()%a ];
		registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");

		if ( whoAmI(gameState) == PLAYER_LORD_GODALMING) {

			while( notHere(north,move,19) == 1) {
				move = locs[ rand()%a ];
				registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			}
		} else if ( whoAmI(gameState) == PLAYER_DR_SEWARD) {

			while( notHere(central,move,19) == 1) {
				move = locs[ rand()%a ];
				registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			}
		} else if ( whoAmI(gameState) == PLAYER_VAN_HELSING) {

			while( notHere(south,move,19) == 1) {
				move = locs[ rand()%a ];
				registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			}
		} else if ( whoAmI(gameState) == PLAYER_MINA_HARKER) {

			while( notHere(east,move,19) == 1) {
				move = locs[ rand()%a ];
				registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
			}
		}

		registerBestPlay(idToAbbrev(move),"I'm on holiday in ...");
	}
}
