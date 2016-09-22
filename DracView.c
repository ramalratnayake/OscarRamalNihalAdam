
// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h" 

#define NUM_TRAPS 2
#define TRAPS 0
#define VAMPS 1
#define CHARS_PER_TURN 7

static void calcTraps(char *pastPlays, DracView dv);     
struct dracView {
    GameView g;
    int minions[NUM_MAP_LOCATIONS][NUM_TRAPS];
};

// Notes:
//      - traps and minions needs fixing
//      - whereIs needs working specifically handling when drac makes special moves
               

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->g = newGameView(pastPlays,messages);
    calcTraps(pastPlays, dracView);
    return dracView;
}
     
     
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}

static void calcTraps(char *pastPlays, DracView dv){
    char *ptr = pastPlays;
    int i=0;
    while(i<NUM_MAP_LOCATIONS){
        dv->minions[i][TRAPS] = 0;
        dv->minions[i][VAMPS] = 0;
        i++;
    }
    if (giveMeTheRound(dv) == 0) {
        return;
    } //Dracula will not have placed any traps yet

    
    while( (*ptr != '\0'  ||  *(&ptr[1]) == '\0') ) {
        if(ptr != pastPlays){
            ptr++; //moves ptr to start of next turn if ptr is not at the start
        }
        char *abrv = malloc(3*sizeof(char));
        abrv[0] = *(ptr+1);
        abrv[1] = *(ptr+2);
        abrv[2] = '\0';
        
        if(*ptr == 'D') {
            if(*(ptr+3) == 'T'){
                dv->minions[abbrevToID(abrv)][TRAPS]++;
            } else if(*(ptr+4) == 'V'){    
                dv->minions[abbrevToID(abrv)][VAMPS]++;
            } else if(*(ptr+5) == 'V'){ //if vampires mature
                LocationID trail[TRAIL_SIZE];
                giveMeTheTrail(dv, PLAYER_DRACULA, trail); //to find out the location of dracula 6 moves ago
                dv->minions[trail[TRAIL_SIZE-1]][VAMPS]--; //which will give the location of the placed vampire
            } else if(*(ptr+5) == 'M'){ //if traps fall off trail
                LocationID trail[TRAIL_SIZE];
                giveMeTheTrail(dv, PLAYER_DRACULA, trail); 
                dv->minions[trail[TRAIL_SIZE-1]][TRAPS]--; 
            }
         }
        
        else{ //hunters encountering the traps
            int i = 0;
            while (i<4){ //4 possible locations for encounters
                if(*(ptr+(2+i)) == 'T'){
                    dv->minions[abbrevToID(abrv)][TRAPS]--;
                } else if(*(ptr+(2+i)) == 'V'){
                    dv->minions[abbrevToID(abrv)][VAMPS]--;
                }
                i++;
            }
        }
        free(abrv);
        ptr += CHARS_PER_TURN;
    } 
}
//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return getRound(currentView->g);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return getScore(currentView->g);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return getHealth(currentView->g,player);
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    // hunters have to infer location of dracula when he makes a double back etc
    // but drac must know where he is 

    int location = getLocation(currentView->g,player);

    if (player == PLAYER_DRACULA) {
        // check teleport, double back, hide
        // more complex decisions can be handled by AIs,
        // rule breaking etc handled by jas's engine im guessing

        int *t = malloc(TRAIL_SIZE*sizeof(int));

        giveMeTheTrail(currentView,player,t);

        if (location == TELEPORT) {
            location = CASTLE_DRACULA;
        } else if (location == DOUBLE_BACK_1) {
            location = t[1];
        } else if (location == DOUBLE_BACK_2) {
            location = t[2];
        } else if (location == DOUBLE_BACK_3) {
            location = t[3];
        } else if (location == DOUBLE_BACK_4) {
            location = t[4];
        } else if (location == DOUBLE_BACK_5) {
            location = t[5];
        }
    }

    return location;
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{

    *end = getLocation(currentView->g,player);

    int *t = malloc(TRAIL_SIZE *sizeof(int));
    getHistory(currentView->g,player,t);
    
    *start = t[1];
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = currentView->minions[where][TRAPS];
    *numVamps = currentView->minions[where][VAMPS];
    return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->g,player,trail);
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    //need to modify connectedLocations, as it requires the input of (int rail) but this function doesnt get 
    //in int rail, as Dracula cannot move via rail
    return connectedLocations(currentView->g,numLocations, whereIs(currentView,PLAYER_DRACULA),
                                        PLAYER_DRACULA, giveMeTheRound(currentView) ,road, FALSE, sea);
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    if(player != PLAYER_DRACULA){
        return connectedLocations(currentView->g,numLocations, whereIs(currentView,player),
                                            player, giveMeTheRound(currentView) , road, rail, sea);
    }else{
        return whereCanIgo(currentView, numLocations, road, sea);
    } 
}


