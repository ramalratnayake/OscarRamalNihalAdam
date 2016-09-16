// GameView.c ... GameView ADT implementation

#include <stdio.h>
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
static void playerLocation(GameView gv, char *pastPlays);
static void lastSix(GameView currentView, char *pastPlays);

struct gameView {
    Round currRound;
    PlayerID currPlayer;
    int score;
    PlayerID trail[NUM_PLAYERS][TRAIL_SIZE];
    int health[NUM_PLAYERS];
    int currLocation[NUM_PLAYERS];
    int location[NUM_PLAYERS][TRAIL_SIZE];
    Map m;
};    

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gv = malloc(sizeof(struct gameView));
    gv->currRound = roundCalc(pastPlays);
    gv->currPlayer = getPlayer(pastPlays); 
    gv->score = calcScore(pastPlays);
    
    int i = 0;
    while(i<NUM_PLAYERS){ 
        int j = 0;
        while(j<TRAIL_SIZE){
            gv->location[i][j]=-1;
            j++;
        }
        i++;
    } 
    lastSix(gv, pastPlays);

    playerLocation(gv,pastPlays);

    gv->m = newMap();

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
/*    char *ptr = &pastPlays[strlen(pastPlays) - 1]; //sets the pointer at the end of the string
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
    */
    if(strlen(pastPlays) == 0){
        return PLAYER_LORD_GODALMING;
    }
    char ptr = pastPlays[strlen(pastPlays) - 7];
    PlayerID player;
    switch(ptr){
        case 'G' : player = PLAYER_DR_SEWARD; break;
        case 'S' : player = PLAYER_VAN_HELSING; break;
        case 'H' : player = PLAYER_MINA_HARKER; break;
        case 'M' : player = PLAYER_DRACULA; break;
        case 'D' : player = PLAYER_LORD_GODALMING; break;
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
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}


static void playerLocation(GameView gv, char *pastPlays) {

    char *ptr = pastPlays;

    if (getRound(gv) == 0) {

        gv->currLocation[PLAYER_LORD_GODALMING] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_DR_SEWARD] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_VAN_HELSING] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_MINA_HARKER] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_DRACULA] = UNKNOWN_LOCATION;
        return;
    }

    while( (*ptr != '\0'  ||  *(&ptr[1]) == '\0') ) {

        if(ptr != pastPlays){
            ptr++; //moves ptr to start of next turn if ptr is not at the start
        }

        if(*ptr == 'G') {
            char *abrv = malloc(3*sizeof(char));
            abrv[0] = *(ptr+1);
            abrv[1] = *(ptr+2);
            abrv[2] = '\0';
            
            gv->currLocation[PLAYER_LORD_GODALMING] = abbrevToID(abrv);
            free(abrv);

        } else if(*ptr == 'S') {
            char *abrv = malloc(3*sizeof(char));
            abrv[0] = *(ptr+1);
            abrv[1] = *(ptr+2);
            abrv[2] = '\0';

            gv->currLocation[PLAYER_DR_SEWARD] = abbrevToID(abrv);
            free(abrv);

        } else if(*ptr == 'H') {
            char *abrv = malloc(3*sizeof(char));
            abrv[0] = *(ptr+1);
            abrv[1] = *(ptr+2);
            abrv[2] = '\0';

            gv->currLocation[PLAYER_VAN_HELSING] = abbrevToID(abrv);
            free(abrv);

        } else if(*ptr == 'M') {
            char *abrv = malloc(3*sizeof(char));
            abrv[0] = *(ptr+1);
            abrv[1] = *(ptr+2);
            abrv[2] = '\0';

            gv->currLocation[PLAYER_MINA_HARKER] = abbrevToID(abrv);
            free(abrv);

        } else if(*ptr == 'D') {
            char *abrv = malloc(3*sizeof(char));
            abrv[0] = *(ptr+1);
            abrv[1] = *(ptr+2);
            abrv[2] = '\0';

            char *cityCheck = "C?";
            char *seaCheck = "S?";
            // check for teleport

            if(strcmp(abrv, cityCheck) == 0 ) {
                gv->currLocation[PLAYER_DRACULA] = CITY_UNKNOWN;
            } else if(strcmp(abrv, seaCheck) == 0 ) {
                gv->currLocation[PLAYER_DRACULA] = SEA_UNKNOWN;
            } else if(abrv[0] == 'D') {
                // double back determined by abrv[1]
                if (abrv[1] == '1') {
                    gv->currLocation[PLAYER_DRACULA] = DOUBLE_BACK_1;
                } else if (abrv[1] == '2') {
                    gv->currLocation[PLAYER_DRACULA] = DOUBLE_BACK_2;
                } else if (abrv[1] == '3') {
                    gv->currLocation[PLAYER_DRACULA] = DOUBLE_BACK_3;
                } else if (abrv[1] == '4') {
                    gv->currLocation[PLAYER_DRACULA] = DOUBLE_BACK_4;
                } else if (abrv[1] == '5') {
                    gv->currLocation[PLAYER_DRACULA] = DOUBLE_BACK_5;
                }
            } else {
                gv->currLocation[PLAYER_DRACULA] = abbrevToID(abrv);
            }
            free(abrv);
        }
        // each player "action" is seperated by 8 spaces, eg "GMN.... SPL.... "
        ptr += CHARS_PER_TURN;

        if(*ptr == 'G') {
            break;
        }
    }
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    //if(currentView->currLocation[player] == UNKNOWN_LOCATION) {
    //    return UNKNOWN_LOCATION;
    //} 

// Otherwise for a hunter it should be an integer in the interval [0..70]
// For dracula it should return his location at the start of the current round
// Possible values for this:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea
//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula
//   LOCATION_UNKNOWN if the round number is 0
    return currentView->currLocation[player];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//fills array recording the last 6 location ids of all players
static void lastSix(GameView currentView, char *pastPlays){
    //if everyone has taken at least 6 turns
    if((currentView->currRound)>=6){
    //goes to the 6th last round
        int index = ((currentView->currRound)-6)*8 + 1;
        PlayerID i = 0;
        while(i<NUM_PLAYERS){
            int turn = 0;
            while(turn<6){
                currentView->location[i][turn]=abbrevToID(&pastPlays[index]);
                turn++;
                index+=40;
            }
            i++;
            index = ((currentView->currRound)-6)*8 + 1 + 8*i;
        }
    } else {
        int index = 1;
        PlayerID i = 0;
        while(i<currentView->currPlayer){
            int turn = 0;
            while(turn<((currentView->currRound)+1)){
                currentView->location[i][turn]=abbrevToID(&pastPlays[index]);
                turn++;
                index+=40;
            }
            i++;
            index = 1 + 8*i;
        }

        while(i<NUM_PLAYERS){
            int turn = 0;
            while(turn<(currentView->currRound)){
                currentView->location[i][turn]=abbrevToID(&pastPlays[index]);
                turn++;
                index+=40;
            }
            i++;
            index = 1 + 8*i;
        }
    } 
}


//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected currLocation

// connectedLocations() returns an array of LocationID that represent
//   all locations that are connected to the given LocationID.
// road, rail and sea are connections should only be considered
//   if the road, rail, sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// Your function must take into account the round and player id for rail travel
// Your function must take into account that Dracula can't move to
//   the hospital or travel by rail but need not take into account Dracula's trail
// The destination 'from' should be included in the array
LocationID *connectedLocations(GameView currentView, int *numcurrLocation,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    // to do
    // start will be using getlocation for the player
    // scan through populating numcurrLocation 
    // can modify map

    int *locs = malloc( (*numcurrLocation)*sizeof(int));

    if (player == PLAYER_DRACULA) {
        // cant move to hospital, or travel by rail
    } else {
        // scan map normally
        connectedLocs(currentView->m,locs,from);
    }

    return locs;
}