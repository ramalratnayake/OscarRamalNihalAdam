// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
// #include "Map.h" ... if you decide to use the Map ADT
     
struct dracView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView g;
};
     

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->g = newGameView(pastPlays,messages);
    return dracView;
}
     
     
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
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
    return getLocation(currentView->g,player);
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
/*	//need to modify connectedLocations, as it requires the input of (int rail) but this function doesnt get 
	//in int rail, as Dracula cannot move via rail
    PlayerID player = whoAmI(currentView);
    return connectedLocations(currentView->g,numLocations, getLocation(currentView->g,player),
										player, getRound(currentView->g) ,road, rail, sea);
*/
	return NULL;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    return connectedLocations(currentView->g,numLocations, getLocation(currentView->g,player),
    									player, getRound(currentView->g) , road, rail, sea);
}
