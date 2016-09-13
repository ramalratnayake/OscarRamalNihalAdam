// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" 

#define CHARS_PER_TURN 7
static Round roundCalc(char *pastPlays);  
static PlayerID getPlayer(char *pastPlays);
   
struct gameView {
    Round round;
    PlayerID currPlayer;
    int score;
    int trail[NUM_PLAYERS][TRAIL_SIZE];
    int health[NUM_PLAYERS];       
};

    

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView gv = malloc(sizeof(struct gameView));
    gv->round = roundCalc(pastPlays);
    gv->currPlayer = getPlayer(pastPlays); 
    return gv;
}
     
     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}

//scans thru string and works out the current round
static Round roundCalc(char *pastPlays){
    char *ptr = pastPlays;
    Round round = 0;  
    while(*ptr != '\0'){
        if(ptr != pastPlays){  //checks for the null terminator b4 moving onto the next turn's info (if statement prevents it happening for the first round) 
            ptr++;
        }
        if(*ptr == 'G'){ //evrytime "G" is  seen, it signifies a new round 
            round++;
        }
        ptr+=CHARS_PER_TURN; //increments ptr to space b4 next turn's info
    }
    return round;
} 

//scans through the last turn info and returns the next player that shud be playing
static PlayerID getPlayer(char *pastPlays){
    char *ptr = &pastPlays[strlen(pastPlays) - 1]; //sets the pointer at the end of the string
    while(*ptr != ' '){
        ptr--;
    } //traverses te list backwards until it finds the 2nd last player
    ptr++; 
    PlayerID player;
    switch(*ptr){
        case 'G' : player = PLAYER_DR_SEWARD;
        case 'S' : player = PLAYER_VAN_HELSING;
        case 'H' : player = PLAYER_MINA_HARKER;
        case 'M' : player = PLAYER_DRACULA;
        case 'D' : player = PLAYER_LORD_GODALMING;
    } //checks the last person that played and returns the next person that should be playing
    return player; 
}
//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current score
int getScore(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}
