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
            gv->location[i][j]=UNKNOWN_LOCATION;
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
//    return currentView->currLocation[player];
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

