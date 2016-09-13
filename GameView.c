// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" 

#define CHARS_PER_TURN 7
#define SAME_PLACE_NEXT_TURN 8
static Round roundCalc(char *pastPlays);  
static PlayerID getPlayer(char *pastPlays);
static int calcScore(char *pastPlays);
   
struct gameView {
    Round currRound;
    PlayerID currPlayer;
    int score;
    int trail[NUM_PLAYERS][TRAIL_SIZE];
    int health[NUM_PLAYERS];       
};

    

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gv = malloc(sizeof(struct gameView));
    gv->currRound = roundCalc(pastPlays);
    gv->currPlayer = getPlayer(pastPlays); 
    gv->score = calcScore(pastPlays);
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
    while(*ptr != '\0' || *(&ptr[1]) == '\0'){ //we arent sure whether a space is added before or after a player turn has commenced
        if(ptr != pastPlays){
            ptr++; //moves ptr to start of next turn if ptr is not at the start
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

static int calcScore(char *pastPlays){
    char *ptr = pastPlays; //pointer to player character
    char *loc = &pastPlays[1]; //pointer to first location character
    char *act = &pastPlays[5]; //pointer to action phase character
    int score = GAME_START_SCORE;

    while(*ptr != '\0' || *(&ptr[1]) == '\0'){
        if(ptr != pastPlays){
            ptr++; //moves ptr to start of next turn if ptr is not at the start
        }

        if(*ptr == 'D'){  //dracula moved
            score -= SCORE_LOSS_DRACULA_TURN; 
        } else if(*ptr == 'D' && *act == 'V'){  //a vampire matured
            score -= SCORE_LOSS_VAMPIRE_MATURES;
        } else if (*ptr != 'D' && *loc == 'J'){  //a hunter is teleported to hospital
            score -= SCORE_LOSS_HUNTER_HOSPITAL;
        }

        ptr += CHARS_PER_TURN; //increments ptr to space b4 next turn's info
        act += SAME_PLACE_NEXT_TURN; //increments the pointer to the same place in the next turn 
        loc += SAME_PLACE_NEXT_TURN;
    }
    return score;
}      
//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->currRound;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->currPlayer;
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
