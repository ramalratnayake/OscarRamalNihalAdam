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
#define MAX_CONNECTIONS 100
#define HUNTER_ENCOUTNER_START 3
#define HUNTER_ENCOUNTER_END 6
#define NUM_HUNTERS NUM_PLAYERS - 1

static Round roundCalc(char *pastPlays);  
static PlayerID getPlayer(char *pastPlays);
static int calcScore(char *pastPlays);
static void playerLocation(GameView gv, char *pastPlays);
static void lastSix(GameView currentView, char *pastPlays);
static PlayerID charToPlayerID(char *ptr);
static void updateHealth(GameView gv, char *pastPlays);
static void checkDoubleBack(char *pastPlays, char*ptr, char **pastLoc);
static void checkHide(char *pastPlays, char* ptr, char **hideLoc);

struct gameView {
    Round currRound;
    PlayerID currPlayer;
    int score;
    PlayerID trail[NUM_PLAYERS][TRAIL_SIZE];
    int health[NUM_PLAYERS];
    int currLocation[NUM_PLAYERS];
    int location[NUM_PLAYERS][TRAIL_SIZE];
    Map m;
    int numDeaths;
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
            gv->location[i][j]=UNKNOWN_LOCATION;
            j++;
        }
        i++;
    } 
    lastSix(gv, pastPlays);

    playerLocation(gv,pastPlays);

    gv->m = newMap();
    
    updateHealth(gv,pastPlays);

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

        if(*ptr == 'D'){ //evrytime "G" is  seen, it signifies a new round 
            round++;
        }
        ptr+=CHARS_PER_TURN; //increments ptr to space b4 next turn's info
    }
    return round;
} 

//scans through the last turn info and returns the next player that shud be playing
static PlayerID getPlayer(char *pastPlays){
    if(strlen(pastPlays) == 0){
        return PLAYER_LORD_GODALMING;
    }
    char ptr = pastPlays[strlen(pastPlays) - CHARS_PER_TURN];
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
        } 
        
        if(*ptr == 'D' && *act == 'V'){  //a vampire matured
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

static PlayerID charToPlayerID(char *ptr){
    PlayerID player;
    switch(*ptr){
        case 'G' : player = PLAYER_LORD_GODALMING; break;
        case 'S' : player = PLAYER_DR_SEWARD ; break;
        case 'H' : player = PLAYER_VAN_HELSING; break;
        case 'M' : player = PLAYER_MINA_HARKER; break;
        case 'D' : player = PLAYER_DRACULA; break;
    } 
    return player;
}
static void updateHealth(GameView gv, char *pastPlays){ 
    char *ptr = pastPlays; //pointer to player character
    int endRoundZero = FALSE; //denotes whether round zero is complete
    int i = 0;
    while (i < NUM_PLAYERS){ //initialize array with corect game start life points
        if (i == PLAYER_DRACULA){
            gv->health[i] = GAME_START_BLOOD_POINTS;
        }else{
            gv->health[i] = GAME_START_HUNTER_LIFE_POINTS;
        }
        i++;
    } 
       
    while(*ptr != '\0' || *(&ptr[1]) == '\0'){
        if(ptr != pastPlays){
            ptr++; //moves ptr to start of next turn if ptr is not at the start
        }
        char *loc = malloc(3*sizeof(char));
                
        if(*ptr == 'D'){  //dracula's move
            endRoundZero = TRUE; //seeing drac move means anything after this is >round 0
            if(ptr[1] == 'D' && atoi((&ptr[2])) <= 5 && atoi((&ptr[2])) >= 1){ //if anything from DD1 to DD5
                char *pastLoc = NULL;
                //pass in a address to the pointer pastLoc so that pastLoc can be altered by the function
                checkDoubleBack(pastPlays, ptr, &pastLoc);
                //pastLoc now stores the start of the turn which contains the actual location 
                loc[0] = *(pastLoc+1);                     
                loc[1] = *(pastLoc+2);
                loc[2] = '\0'; //store the location abbreviation in a string 
            }else if(ptr[1] == 'H' && ptr[2] == 'I'){
                char *lastLoc;
                checkHide(pastPlays, ptr, &lastLoc);               
                loc[0] = *(lastLoc+1);                     
                loc[1] = *(lastLoc+2);
                loc[2] = '\0'; //store the location abbreviation in a string
            }else{
                loc[0] = *(ptr+1);
                loc[1] = *(ptr+2);
                loc[2] = '\0'; //store the location abbreviation in a string 
            } 
            //finds out what that location is and updates health in struct      
            if(strcmp(loc,"S?") == 0){
                gv->health[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
            }else if (strcmp(loc,"C?") == 0){
                //dont do anything, prevents passing C? and getting error from abbrevToId()
            }else if (strcmp(loc,"TP") == 0){
                gv->health[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA; //gets teleported to castle            
            }else if (idToType(abbrevToID(loc)) == SEA){
                gv->health[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
            }else if (abbrevToID(loc) == CASTLE_DRACULA){
                gv->health[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
            }
        }else{ //a hunter's move
            i = HUNTER_ENCOUTNER_START;
            while (i <= HUNTER_ENCOUNTER_END){ //scanning the range of chars that hold encounters for the hunters
                switch (*(&ptr[i])){
                    case 'T' : gv->health[charToPlayerID(ptr)] -= LIFE_LOSS_TRAP_ENCOUNTER; break;
                    case 'D' : gv->health[charToPlayerID(ptr)] -= LIFE_LOSS_DRACULA_ENCOUNTER; 
                               gv->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER; break;
                }
                i++;
            }

            if(endRoundZero == TRUE){ //cannot check location of previous round unless round > 0
                char *prev = ptr - (SAME_PLACE_NEXT_TURN * NUM_PLAYERS); //pointer to the start of the player's turn in prev round 
                char *prevLoc = malloc(3*sizeof(char));
                prevLoc[0] = *(prev+1);
                prevLoc[1] = *(prev+2);
                prevLoc[2] = '\0'; //store the location abbreviation in a string   
                if(abbrevToID(prevLoc) == abbrevToID(loc)){ //hunter has been in the same place for the previous round as well
                    gv->health[charToPlayerID(ptr)] += LIFE_GAIN_REST;
                }
                free(prevLoc);
            }
        }                
                
        free(loc); 
    
        i = 0;
        while(i < NUM_HUNTERS){ //counting dead hunters and resetting their life points and placing upper limit to life points
            if (gv->health[i] <= 0){
                gv->numDeaths ++;
                gv->health[i] = GAME_START_HUNTER_LIFE_POINTS;
            } else if (gv->health[i] > GAME_START_HUNTER_LIFE_POINTS){
                gv->health[i] = GAME_START_HUNTER_LIFE_POINTS;
            }
            i++;
        } 
                   
        ptr += CHARS_PER_TURN; //increments ptr to space b4 next turn's info
        
    }   
    
    return;
}

//checks the Double Back locations and updates the pointer
static void checkDoubleBack(char *pastPlays, char* ptr, char **pastLoc){
    char *back = ptr - (SAME_PLACE_NEXT_TURN * NUM_PLAYERS * atoi((&ptr[2]))); 
    if(back[1] == 'H' && back[2] == 'I'){
        *pastLoc = back - (SAME_PLACE_NEXT_TURN * NUM_PLAYERS); //saves the ppointer to the turn which contained the correct location of double back
    }else{
        *pastLoc = back;
    }     
    return;   
}    
    
static void checkHide(char *pastPlays, char* ptr, char **hideLoc){
    char *lastLoc = ptr - (SAME_PLACE_NEXT_TURN * NUM_PLAYERS); //go back a round
    if(lastLoc[1] == 'D' && atoi((&lastLoc[2])) <= 5 && atoi((&lastLoc[2])) >= 1){     
        char *dBackLoc;
        checkDoubleBack(pastPlays, lastLoc, &dBackLoc);
        
        *hideLoc = dBackLoc;
    }else{
        *hideLoc = lastLoc;
    }
    return;
}
    
// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return currentView->health[player];
}

// Get the current health points for a given player


static void playerLocation(GameView gv, char *pastPlays) {

    char *ptr = pastPlays;

    if (getRound(gv) == 0) {
        gv->currLocation[PLAYER_LORD_GODALMING] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_DR_SEWARD] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_VAN_HELSING] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_MINA_HARKER] = UNKNOWN_LOCATION;
        gv->currLocation[PLAYER_DRACULA] = UNKNOWN_LOCATION;
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
            char *teleportCheck = "TP";
            char *hideCheck = "HI";

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
            } else if (strcmp(abrv, teleportCheck) == 0 ) {
                gv->currLocation[PLAYER_DRACULA] = TELEPORT;
            } else if (strcmp(abrv, hideCheck) == 0 ) {
                gv->currLocation[PLAYER_DRACULA] = HIDE;
            } else {
                gv->currLocation[PLAYER_DRACULA] = abbrevToID(abrv);
            }
            free(abrv);
        }
        ptr += CHARS_PER_TURN;

        if(*ptr == 'G') {
            break;
        }
    }
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{    
    return currentView->location[player][0];
}

//// Functions that return information about the history of the game
// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    int i = 0;
    while(i<TRAIL_SIZE){
        trail[i]=currentView->location[player][i];
        i++;
    }
}


//fills array recording the last 6 location ids of all players
static void lastSix(GameView currentView, char *pastPlays){
    //if at least 6 rounds
    if((currentView->currRound)>=6){
        //goes to the 6th last round
        int index = ((currentView->currRound)-6)*SAME_PLACE_NEXT_TURN*NUM_PLAYERS + 1;
        PlayerID i = 0;
        //if some players has taken more turns than others
        //for which, go to next turn (since they have one more turn) 
        while(i<currentView->currPlayer){
            int turn = 0;
            index+=SAME_PLACE_NEXT_TURN*NUM_PLAYERS;
            while(turn<TRAIL_SIZE){
                if(pastPlays[index]=='C' && pastPlays[index+1]=='?'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = CITY_UNKNOWN;
                } else if(pastPlays[index]=='S' && pastPlays[index+1]=='?'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = SEA_UNKNOWN;
                } else if(pastPlays[index]=='H' && pastPlays[index+1]=='I'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = HIDE;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='1'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_1;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='2'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_2;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='3'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_3;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='4'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_4;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='5'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_5;
                } else if(pastPlays[index]=='T' && pastPlays[index+1]=='P'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = TELEPORT;
                } else {
                    currentView->location[i][TRAIL_SIZE-1-turn]=abbrevToID(&pastPlays[index]);
                }
                turn++;
                index+=SAME_PLACE_NEXT_TURN*NUM_PLAYERS;
            }
            i++;
            //reset index for next playerID
            index = ((currentView->currRound)-6)*SAME_PLACE_NEXT_TURN*NUM_PLAYERS + 1 + 8*i;
        }
        //for players that have as many turns as the current round number
        while(i<NUM_PLAYERS){
            int turn = 0;
            while(turn<TRAIL_SIZE){
                if(pastPlays[index]=='C' && pastPlays[index+1]=='?'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = CITY_UNKNOWN;
                } else if(pastPlays[index]=='S' && pastPlays[index+1]=='?'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = SEA_UNKNOWN;
                } else if(pastPlays[index]=='H' && pastPlays[index+1]=='I'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = HIDE;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='1'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_1;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='2'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_2;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='3'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_3;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='4'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_4;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='5'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = DOUBLE_BACK_5;
                } else if(pastPlays[index]=='T' && pastPlays[index+1]=='P'){
                    currentView->location[i][TRAIL_SIZE-1-turn] = TELEPORT;
                } else {
                    currentView->location[i][TRAIL_SIZE-1-turn]=abbrevToID(&pastPlays[index]);
                }
                turn++;
                index+=SAME_PLACE_NEXT_TURN*NUM_PLAYERS;
            }
            i++;
            index = ((currentView->currRound)-6)*SAME_PLACE_NEXT_TURN*NUM_PLAYERS + 1 + 8*i;
        }
    } else {
        //when round number<6, so less than 6 moves have been made for more than one player
        int index = 1;
        PlayerID i = 0;
        while(i<currentView->currPlayer){
            int turn = 0;
            while(turn<((currentView->currRound)+1)){
                if(pastPlays[index]=='C' && pastPlays[index+1]=='?'){
                    currentView->location[i][currentView->currRound-turn] = CITY_UNKNOWN;
                } else if(pastPlays[index]=='S' && pastPlays[index+1]=='?'){
                    currentView->location[i][currentView->currRound-turn] = SEA_UNKNOWN;
                } else if(pastPlays[index]=='H' && pastPlays[index+1]=='I'){
                    currentView->location[i][currentView->currRound-turn] = HIDE;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='1'){
                    currentView->location[i][currentView->currRound-turn] = DOUBLE_BACK_1;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='2'){
                    currentView->location[i][currentView->currRound-turn] = DOUBLE_BACK_2;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='3'){
                    currentView->location[i][currentView->currRound-turn] = DOUBLE_BACK_3;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='4'){
                    currentView->location[i][currentView->currRound-turn] = DOUBLE_BACK_4;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='5'){
                    currentView->location[i][currentView->currRound-turn] = DOUBLE_BACK_5;
                } else if(pastPlays[index]=='T' && pastPlays[index+1]=='P'){
                    currentView->location[i][currentView->currRound-turn] = TELEPORT;
                } else {
                    currentView->location[i][currentView->currRound-turn]=abbrevToID(&pastPlays[index]);
                }
                turn++;
                index+=SAME_PLACE_NEXT_TURN*NUM_PLAYERS;
            }
            i++;
            index = 1 + SAME_PLACE_NEXT_TURN*i;
        }
        while(i<NUM_PLAYERS){
            int turn = 0;
            while(turn<(currentView->currRound)){
                if(pastPlays[index]=='C' && pastPlays[index+1]=='?'){
                    currentView->location[i][currentView->currRound-turn-1] = CITY_UNKNOWN;
                } else if(pastPlays[index]=='S' && pastPlays[index+1]=='?'){
                    currentView->location[i][currentView->currRound-turn-1] = SEA_UNKNOWN;
                } else if(pastPlays[index]=='H' && pastPlays[index+1]=='I'){
                    currentView->location[i][currentView->currRound-turn-1] = HIDE;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='1'){
                    currentView->location[i][currentView->currRound-turn-1] = DOUBLE_BACK_1;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='2'){
                    currentView->location[i][currentView->currRound-turn-1] = DOUBLE_BACK_2;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='3'){
                    currentView->location[i][currentView->currRound-turn-1] = DOUBLE_BACK_3;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='4'){
                    currentView->location[i][currentView->currRound-turn-1] = DOUBLE_BACK_4;
                } else if(pastPlays[index]=='D' && pastPlays[index+1]=='5'){
                    currentView->location[i][currentView->currRound-turn-1] = DOUBLE_BACK_5;
                } else if(pastPlays[index]=='T' && pastPlays[index+1]=='P'){
                    currentView->location[i][currentView->currRound-turn-1] = TELEPORT;
                } else {
                    currentView->location[i][currentView->currRound-turn-1]=abbrevToID(&pastPlays[index]);
                }
                turn++;
                index+=SAME_PLACE_NEXT_TURN*NUM_PLAYERS;
            }
            i++;
            index = 1 + SAME_PLACE_NEXT_TURN*i;
        }
    }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected currLocation
LocationID *connectedLocations(GameView currentView, int *numcurrLocation,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    LocationID *locs = malloc( MAX_CONNECTIONS *sizeof(int));

    if (player == PLAYER_DRACULA) {
        // cant travel by rail
        // exclude all locations in his trail expect curr
        // exclude hospital
        LocationID *dracTrail = malloc(TRAIL_SIZE*sizeof(int));
        
        int i = 0;
        while(i < TRAIL_SIZE) {
            dracTrail[i] = currentView->trail[PLAYER_DRACULA][i];
            i++;
        }

        *numcurrLocation = connectedLocs(currentView->m,locs,from,player,round,road,0,sea,dracTrail);

        free(dracTrail);

    } else {
        *numcurrLocation = connectedLocs(currentView->m,locs,from,player,round,road,rail,sea,NULL);
    }

    return locs;
}
