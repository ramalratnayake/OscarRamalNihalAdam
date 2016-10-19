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

   - better results from fanning out and searching rather than relying
     on resting

   - 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Current Strategy:
   - If dracs loc is known take the shortest path to get there
   - else back trace his movements reducing the list of possible locs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Strategy ideas:
   - rank locations based of connected locations / probability
   - determine location of vampires and destroy them when they appear
   - compare functions
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
#define ERROR -1
#define MAX_LOCS 70
#define OKAY 1
#define NOT_OKAY 0
#define NEED_REST 80085

// MapLoc ADT's used are thanks to jas
// jas@cse.unsw.edu.au
typedef struct _mapLoc {
      char *name;
      char *abbrev;
      int id;
      int type;
} MapLoc;

static int makeRandomMove(HunterView hv,int player);
static int notHere(int *locs, int x, int size);
static int checkTrail(HunterView hv, int player,int move,int version);
static int checkForOthers(HunterView hv, int player);
static int predictingDracLocs(HunterView hv);
static int connectedWithinRange(HunterView hv,int from,int to,int count,int road,int boat);
static int backTraceElimination(HunterView hv);
static void showArray(int *a,int l);

void decideHunterMove(HunterView gameState)
{
   srand(time(NULL));
   int player = whoAmI(gameState);
   int currLoc = whereIs(gameState,player);
   //int round = giveMeTheRound(gameState);

   if(currLoc == NOWHERE){
        if(player == PLAYER_LORD_GODALMING){
            registerBestPlay(idToAbbrev(EDINBURGH),"Starting hunt in Edinburgh...");
        } else if(player == PLAYER_DR_SEWARD){
            registerBestPlay(idToAbbrev(CADIZ),"Starting hunt in Cadiz...");
        } else if(player == PLAYER_VAN_HELSING){
            registerBestPlay(idToAbbrev(SZEGED),"Starting hunt in Szeged...");
        } else if(player == PLAYER_MINA_HARKER){
            registerBestPlay(idToAbbrev(BARI),"Starting hunt in Bari...");
        }
   } else {
      if (howHealthyIs(gameState, player ) < 4 ) {
        registerBestPlay(idToAbbrev(currLoc),"I'm hurt badly, resting...");
      } /*else if (round%6 == 0) {
        registerBestPlay(idToAbbrev(currLoc),"Researching...");
      } */else {
         int move = -1;
         int a;
         int *locs = whereCanIgo(gameState, &a, 1,1,1);
         int dracCurrLoc = whereIs(gameState,PLAYER_DRACULA);

         //============================================================
         // Dracula location is known -> goes towards it
         //============================================================
         
         if(validPlace(dracCurrLoc) == TRUE) {
            if(currLoc == dracCurrLoc) {
               int predict = predictingDracLocs(gameState);
               if(predict != -1) {
                  move = predict;
                  registerBestPlay(idToAbbrev(move),"Predicting...");
               }
            } else {
               int length;               
               int *p = minPathFinder(gameState,currLoc,dracCurrLoc,&length,1,1,1);
               
               if (notHere(locs,p[1],a) == 0) {
                  move = p[1];
                  registerBestPlay(idToAbbrev(move),"I know where drac is, going the shortest path!");
               }
            }
         } 

         //============================================================
         // Dracula location is unknown -> goes towards valid trail loc
         //============================================================
         else {
            int check = backTraceElimination(gameState);

            if(validPlace(check) == TRUE) {
               int length;               
               int *p = minPathFinder(gameState,currLoc,check,&length,1,1,1);
               if (notHere(locs,p[1],a) == 0) {
                  move = p[1];
                  registerBestPlay(idToAbbrev(move),"Hunting Drac's trail using the shortest path I know!");
               }
            }

            if( validPlace(move) != TRUE ) {
               move = makeRandomMove(gameState,player);
               int i = 0;
               while(i <= MAX_ITERATIONS) { 
                  if(checkTrail(gameState,player,move,1) == 0 && checkForOthers(gameState,player) == 1) {
                     break;
                  }
                  move = makeRandomMove(gameState,player);
                  i++;
               }
                            
               registerBestPlay(idToAbbrev(move),"Patrolling randomly..."); 
            }
         }  
      }
   }
}

static int makeRandomMove(HunterView hv,int player) 
{
   int a;
   int *locs = whereCanIgo(hv, &a, 1,1,1);
   
   int r = rand()%a;
   return locs[r];
}

static int notHere(int *locs, int x, int size) 
{
   int i = 0;
   while(i < size) {
      if(locs[i] == x) {
         // 0 -> found         
         return FALSE;
      }
      i++;
   }
   // 1 -> not found
   return TRUE;
}

static int checkTrail(HunterView hv,int player,int move,int version) 
{
   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,player,t);
   
   if (version == 1) {

      if (notHere(t,move,TRAIL_SIZE) == 1) {
         return FALSE;
      } else {
         return TRUE;
      } 
   } else {
      // checks prev 2 moves
      if (move == t[0] || move == t[1]) {
         return FALSE;
      } else {
         return TRUE;
      }
   }   
}

static int checkForOthers(HunterView hv,int player) 
{
   int g = whereIs(hv,PLAYER_LORD_GODALMING);
   int s = whereIs(hv,PLAYER_DR_SEWARD); 
   int v = whereIs(hv,PLAYER_VAN_HELSING); 
   int m = whereIs(hv,PLAYER_MINA_HARKER);

   int p = whereIs(hv,player);

   if(player ==  PLAYER_LORD_GODALMING){
      if(p == s || p == v || p == m) {
         return FALSE;
      } else {
         return TRUE;
      }
   } else if(player ==  PLAYER_DR_SEWARD){
      if(p == g || p == v || p == m) {
         return FALSE;
      } else {
         return TRUE;
      }
   } else if(player ==  PLAYER_VAN_HELSING){
      if(p == s || p == g || p == m) {
         return FALSE;
      } else {
         return TRUE;
      }
   } else if(player ==  PLAYER_MINA_HARKER){
      if(p == s || p == v || p == g) {
         return FALSE;
      } else {
         return TRUE;
      }
   }
   return ERROR;
}

static int predictingDracLocs(HunterView hv) 
{
   int n,a;
   int *dracLocs = whereCanTheyGo(hv,&n,PLAYER_DRACULA,1,0,1);
   int *hunterLocs = whereCanIgo(hv,&a,1,1,1);

   int dracCurrLoc = whereIs(hv,PLAYER_DRACULA);

   if(validPlace(dracCurrLoc) != TRUE) {
      return ERROR;
   } else {
      int x = 0;
      while(x < a) {
         if (notHere(dracLocs,hunterLocs[x],n) == FALSE) {
            return hunterLocs[x];
         }
         x++;
      }
      return ERROR;
   }
}

static int connectedWithinRange(HunterView hv,int from,int to,int count,int road,int boat) 
{
   // shortest path with give the same affect as within range
   // consider rails   
   if(validPlace(from) == TRUE && validPlace(to) == TRUE) {
      int length;
      int *p = minPathFinder(hv,from,to,&length,0,road,boat);
      
      free(p);

      if (length <= count) {
         return TRUE;
      } else {
         return FALSE;
      }
   } else {
      return FALSE;
   } 
}

static int backTraceElimination(HunterView hv) 
{   
   // scan through places[]
   MapLoc places[] = {
      {"Adriatic Sea", "AS", ADRIATIC_SEA, SEA},
      {"Alicante", "AL", ALICANTE, LAND},
      {"Amsterdam", "AM", AMSTERDAM, LAND},
      {"Athens", "AT", ATHENS, LAND},
      {"Atlantic Ocean", "AO", ATLANTIC_OCEAN, SEA},
      {"Barcelona", "BA", BARCELONA, LAND},
      {"Bari", "BI", BARI, LAND},
      {"Bay of Biscay", "BB", BAY_OF_BISCAY, SEA},
      {"Belgrade", "BE", BELGRADE, LAND},
      {"Berlin", "BR", BERLIN, LAND},
      {"Black Sea", "BS", BLACK_SEA, SEA},
      {"Bordeaux", "BO", BORDEAUX, LAND},
      {"Brussels", "BU", BRUSSELS, LAND},
      {"Bucharest", "BC", BUCHAREST, LAND},
      {"Budapest", "BD", BUDAPEST, LAND},
      {"Cadiz", "CA", CADIZ, LAND},
      {"Cagliari", "CG", CAGLIARI, LAND},
      {"Castle Dracula", "CD", CASTLE_DRACULA, LAND},
      {"Clermont-Ferrand", "CF", CLERMONT_FERRAND, LAND},
      {"Cologne", "CO", COLOGNE, LAND},
      {"Constanta", "CN", CONSTANTA, LAND},
      {"Dublin", "DU", DUBLIN, LAND},
      {"Edinburgh", "ED", EDINBURGH, LAND},
      {"English Channel", "EC", ENGLISH_CHANNEL, SEA},
      {"Florence", "FL", FLORENCE, LAND},
      {"Frankfurt", "FR", FRANKFURT, LAND},
      {"Galatz", "GA", GALATZ, LAND},
      {"Galway", "GW", GALWAY, LAND},
      {"Geneva", "GE", GENEVA, LAND},
      {"Genoa", "GO", GENOA, LAND},
      {"Granada", "GR", GRANADA, LAND},
      {"Hamburg", "HA", HAMBURG, LAND},
      {"Ionian Sea", "IO", IONIAN_SEA, SEA},
      {"Irish Sea", "IR", IRISH_SEA, SEA},
      {"Klausenburg", "KL", KLAUSENBURG, LAND},
      {"Le Havre", "LE", LE_HAVRE, LAND},
      {"Leipzig", "LI", LEIPZIG, LAND},
      {"Lisbon", "LS", LISBON, LAND},
      {"Liverpool", "LV", LIVERPOOL, LAND},
      {"London", "LO", LONDON, LAND},
      {"Madrid", "MA", MADRID, LAND},
      {"Manchester", "MN", MANCHESTER, LAND},
      {"Marseilles", "MR", MARSEILLES, LAND},
      {"Mediterranean Sea", "MS", MEDITERRANEAN_SEA, SEA},
      {"Milan", "MI", MILAN, LAND},
      {"Munich", "MU", MUNICH, LAND},
      {"Nantes", "NA", NANTES, LAND},
      {"Naples", "NP", NAPLES, LAND},
      {"North Sea", "NS", NORTH_SEA, SEA},
      {"Nuremburg", "NU", NUREMBURG, LAND},
      {"Paris", "PA", PARIS, LAND},
      {"Plymouth", "PL", PLYMOUTH, LAND},
      {"Prague", "PR", PRAGUE, LAND},
      {"Rome", "RO", ROME, LAND},
      {"Salonica", "SA", SALONICA, LAND},
      {"Santander", "SN", SANTANDER, LAND},
      {"Saragossa", "SR", SARAGOSSA, LAND},
      {"Sarajevo", "SJ", SARAJEVO, LAND},
      {"Sofia", "SO", SOFIA, LAND},
      {"St Joseph and St Marys", "JM", ST_JOSEPH_AND_ST_MARYS, LAND},
      {"Strasbourg", "ST", STRASBOURG, LAND},
      {"Swansea", "SW", SWANSEA, LAND},
      {"Szeged", "SZ", SZEGED, LAND},
      {"Toulouse", "TO", TOULOUSE, LAND},
      {"Tyrrhenian Sea", "TS", TYRRHENIAN_SEA, SEA},
      {"Valona", "VA", VALONA, LAND},
      {"Varna", "VR", VARNA, LAND},
      {"Venice", "VE", VENICE, LAND},
      {"Vienna", "VI", VIENNA, LAND},
      {"Zagreb", "ZA", ZAGREB, LAND},
      {"Zurich", "ZU", ZURICH, LAND},
   };
   
   int dracCurrLoc = whereIs(hv,PLAYER_DRACULA);
   int *possibleLocs = malloc(MAX_LOCS*sizeof(int));
   int a = 0;
   while(a < MAX_LOCS) {
      possibleLocs[a] = UNKNOWN_LOCATION;
      a++;
   }

   int *t = malloc(TRAIL_SIZE*sizeof(int));
   giveMeTheTrail(hv,PLAYER_DRACULA,t);

   /*
   int g = 0;
   int vCheck = NOT_OKAY;
   while(g <= TRAIL_SIZE) {
      if(validPlace(t[g]) == TRUE) {
         vCheck = OKAY;         
         break;
      }
      g++;
   }

   if(vCheck == NOT_OKAY) {
      return NEED_REST;
   }  

   if( (t[0] == CITY_UNKNOWN || t[0] == SEA_UNKNOWN) 
    && (t[1] == CITY_UNKNOWN || t[1] == SEA_UNKNOWN)
    && (t[2] == CITY_UNKNOWN || t[2] == SEA_UNKNOWN)
    && (t[3] != -1) && (t[4] != -1) && (t[5] != -1) ) {
      return NEED_REST;
   }*/

   int result = -1;
   int x = 0;
   int count = 1;

   printf("Trail:\n");
   showArray(t,TRAIL_SIZE);

   while(x < TRAIL_SIZE) {
      if (validPlace(t[x]) == TRUE) {
         result = t[x];         
         break;
      } else if (validPlace(t[x]) != TRUE) {
         if(t[x] == HIDE) {
            count--;
         }
         // can only DD to locs on his trail and connected locs
      }
      x++;
      count++;
   }   

   //printf("Most recent: %s\n",idToName(result));
   printf("Count: %d\n",count);
   int j = 0;
   // eliminate everything not within "count" connected locs
   if(validPlace(result) == TRUE) {
      int z = 1;
      int pathCheck = 0;
      while(z < x && validPlace(t[z]) != TRUE ) {
         if(t[z-1] == t[z]) {
            pathCheck = 1;
         }
         z++;
      }
      int road = 1;
      int boat = 1;

      if(pathCheck == 1) {
         if(t[x] == CITY_UNKNOWN) {
            road = 1;
            boat = 0;
         } else if(t[x] == SEA_UNKNOWN) {
            road = 0;
            boat = 1;
         }
      }
   
      int i = 0;
      while(i <= MAX_LOCS) {
         if(connectedWithinRange(hv,result,places[i].id,count,road,boat) == TRUE) {
            if(dracCurrLoc == CITY_UNKNOWN && places[i].type == LAND) {
               possibleLocs[j] = places[i].id;
               j++;            
            } else if(dracCurrLoc == SEA_UNKNOWN && places[i].type == SEA) {
               possibleLocs[j] = places[i].id;
               j++;
            } else if(dracCurrLoc == HIDE) {
               possibleLocs[j] = places[i].id;
               j++;
            }
         }
         i++;
      }
   }
   printf("Possible locations:\n");
   showArray(possibleLocs,j);

   // check for hides and double backs
   // ...

   // if no sea trips taken eliminate that path
   // " " for cities
   // ...


   // choose the location that is closes to the hunter   
   int currLoc = whereIs(hv,whoAmI(hv));
   
   if(validPlace(possibleLocs[0]) == TRUE) {
      int check;
      int *p = minPathFinder(hv,currLoc,possibleLocs[0],&check,1,1,1);
      free(p);
   
      int i = 1;
      int place = possibleLocs[0];
      while(i < j) {
         int l;
         int *p = minPathFinder(hv,currLoc,possibleLocs[i],&l,1,1,1);
         free(p);    

         if(l > check) {
            check = l;
            place = possibleLocs[i];
         }
         i++;
      }
      //printf("Choosing %s\n",idToName(place));
      return place;
   } else {
      // check for immature vampires

      return UNKNOWN_LOCATION;
   }
}

static void showArray(int *a, int l) 
{
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