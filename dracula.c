// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "Game.h"
#include "DracView.h"
#include <time.h>

#define DOUBLE_BACK 95959
#define NOT_FOUND 899745

int isInTrail (DracView gameState, LocationID move);
int isValidMove(DracView gameState, LocationID move);
int closeToHunters(DracView gameState, LocationID move);
void makeDoubleBack(DracView dv);
static void printStuff(DracView gameState);
int inShortestPath(int move,  DracView dv);
int findMaximum(int a[], int n);
int chooseFurtherst(DracView dv);
static int minimum(int num1, int num2, int num3, int num4);

static int notHere(int *locs, int x, int size) {
   int i = 0;
   while(i < size) {
      if(locs[i] == x) {
         return FALSE;
      }
      i++;
   }
   return TRUE;
}

void decideDraculaMove(DracView gameState)
{
    
    if(giveMeTheRound(gameState) == 0){
        registerBestPlay(idToAbbrev(chooseFurtherst(gameState)),"Mwuhahahaha");
    } else if(giveMeTheRound(gameState)%26 == 0 && giveMeTheRound(gameState) > 12 && isInTrail (gameState, HIDE) != TRUE && idToType(whereIs(gameState, PLAYER_DRACULA)) != SEA){
        printStuff(gameState);
        registerBestPlay("HI","Do not react...."); //sneaky move to lure hunters to a trap
    }else {
        printStuff(gameState);
        int i = 0;
        int index;
        int move;
        int found = FALSE;
        int a;
        int *locs = whereCanIgo(gameState, &a, 1,1);
        if(a != 0){
            index = giveMeTheRound(gameState)%(a);
            while (i<(4*a)){ //try to find a perfect move first, then an OK move, then a retarded one 
                move = locs[index];
                if(i>=3*a){ //a retarded move: one that is legal
                    if(isValidMove(gameState, move) == TRUE){
                        found = TRUE;
                        registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
                        printf ("my retarded move is %s\n", idToName(move));
                        break;
                    }
                }else if (i>=2*a){ //an OK move: not in the sea OR not adjacent to any hunters
                    if(isValidMove(gameState, move) == TRUE && (idToType(move) != SEA || closeToHunters(gameState, move) == FALSE)){
                        found = TRUE;
                        registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
                        printf ("my semi-genius move is %s\n", idToName(move));
                        break;
                    }
                }else if (i>=a){ //a perfect move: not in the sea AND not adjacent to any hunters
                    if(isValidMove(gameState, move) == TRUE && closeToHunters(gameState, move) == FALSE && idToType(move) != SEA){
                        found = TRUE;
                        registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
                        printf ("my genius move is %s\n", idToName(move));
                        break;
                    }
                }else{ //a perfect move: not in the sea AND not adjacent to any hunters
                    if(isValidMove(gameState, move) == TRUE && closeToHunters(gameState, move) == FALSE && idToType(move) != SEA &&  inShortestPath(move, gameState)==FALSE){
                        found = TRUE;
                        registerBestPlay(idToAbbrev(move),"Mwuhahahaha");
                        printf ("my ultra genius move is %s\n", idToName(move));
                        break;
                    }
                }
                i++; 
                index++;
                index = index%a;
            }
        }
        if(found == FALSE){ //not found anything
            if(isInTrail (gameState, HIDE) != TRUE && idToType(whereIs(gameState, PLAYER_DRACULA)) != SEA ){
                registerBestPlay("HI", "Goodbye"); //make a HIDE
            }else if(isInTrail (gameState, DOUBLE_BACK) != TRUE){
                makeDoubleBack(gameState);
                //registerBestPlay("D3", "Goodbye"); 
            }else{
                registerBestPlay("TP", "Goodbye"); //otherwise, no choice but to Teleport
            }
        }

        // checks for hunter locations to make a better decision
        

    }
}

int isInTrail (DracView gameState, LocationID move){
    LocationID trail [TRAIL_SIZE];
    giveMeTheTrail(gameState,PLAYER_DRACULA,trail);
    int  i = 0;
    if (move == HIDE){ 
        if(haveIHidden(gameState) == TRUE) {
            return TRUE;
        }  
    }else if(move == DOUBLE_BACK){ 
        if(haveIDoubleBacked(gameState) == TRUE){
            return TRUE;
            printf("back to the double\n");
        }
    }else{
        while (i< TRAIL_SIZE-1){
            if (move == trail[i]){
                int j = i+1;
                while(j<TRAIL_SIZE){
                    if(move == trail[j] && j != TRAIL_SIZE - 1){
                        return TRUE;
                    }else if(move == trail[j] && j == TRAIL_SIZE - 1){
                        return FALSE;
                    }
                    j++;
                }
                return TRUE;
            }
            i++;
        }
    }
    
    return FALSE;
}
    
int isValidMove(DracView gameState, LocationID move){
    if(isInTrail (gameState, move) == TRUE){
        return FALSE;
    }else if (move == ST_JOSEPH_AND_ST_MARYS){
        return FALSE;
    }else if (move == whereIs(gameState, PLAYER_DRACULA)){
        return FALSE;
    }
    return TRUE;
}

int closeToHunters(DracView gameState, LocationID move){
    int a;
    int b;
    int c;
    int d;
    int *locPlayer0 = whereCanTheyGo(gameState,&a,PLAYER_LORD_GODALMING, 1,1,1);
    int *locPlayer1 = whereCanTheyGo(gameState,&b,PLAYER_DR_SEWARD, 1,1,1);
    int *locPlayer2 = whereCanTheyGo(gameState,&c,PLAYER_VAN_HELSING, 1,1,1);
    int *locPlayer3 = whereCanTheyGo(gameState,&d,PLAYER_MINA_HARKER, 1,1,1);

    if (notHere(locPlayer0,move,a) == FALSE) {
        return TRUE;
    } else if (notHere(locPlayer1,move,b) == FALSE){
        return TRUE;
    } else if (notHere(locPlayer2,move,c) == FALSE){
        return TRUE;
    } else if (notHere(locPlayer3,move,d) == FALSE){
        return TRUE;
    }    
    return FALSE;
}
      
void makeDoubleBack(DracView dv){
    LocationID trail [TRAIL_SIZE];
    printf("making double back\n");
    giveMeTheTrail(dv,PLAYER_DRACULA,trail); 
    int a;
    int *locs = whereCanIgo(dv, &a, 1,1);
    int i = 0;
    int found = FALSE;
    int level = 1; 
    while(level <= 3){
        i=0; //when calling shortest length mk sure round is grtr than one
        while(i<(TRAIL_SIZE-1)){
            if(level == 1 && closeToHunters(dv, trail[i]) == FALSE && idToType(trail[i]) != SEA && notHere(locs,trail[i],a) == FALSE){
                found = TRUE;
                break;
            }else if(level == 2 && (closeToHunters(dv, trail[i]) == FALSE || idToType(trail[i]) != SEA) && notHere(locs,trail[i],a) == FALSE){
                found = TRUE;
                break;
            }else if(level == 3 && notHere(locs,trail[i],a) == FALSE && idToType(trail[i]) != SEA){
                found = TRUE;
                break;
            }
            i++;
        }
        if(found == TRUE) break;
        level++;
    }
    // i will be at the index of the "good" move
    i=i%TRAIL_SIZE;
    //if(found == TRUE){
    switch(i){  
        case 0:
            registerBestPlay("D1", "Mwuhahaha");
            return;
        case 1:
            registerBestPlay("D2","Mwuhahahaha");
            return;                   
        case 2:
            registerBestPlay("D3","Mwuhahahaha");
            return;
        case 3:
            registerBestPlay("D4","Mwuhahahaha");
            return;
        case 4:
            registerBestPlay("D5","Mwuhahahaha");
            return;
    }
    //}
    /*
    else{ //if a good move hasn't been found, make a random double back
        i = 1;
        while (i <= TRAIL_SIZE-2){
            if (notHere(locs,trail[i],a) == FALSE){
                break;
            }
            i++;
        }
        switch(i){ 
            case 0:
                registerBestPlay("D1","Mwuhahahaha"); //need to change to D1 later
                break;            
            case 1:
                registerBestPlay("D2","Mwuhahahaha");
                break;                   
            case 2:
                registerBestPlay("D3","Mwuhahahaha");
                break;
            case 3:
                registerBestPlay("D4","Mwuhahahaha");
                break;
            case 4:
                registerBestPlay("D5","Mwuhahahaha");
                break;
        }     
    }
    */
    registerBestPlay("D1", "Lololol");
    printf("failsafe brah\n");
    return;
}  

static void printStuff(DracView gameState){ 
    //printing stuff
    //...............................................................
    //printf("%d\n", giveMeTheRound(gameState));
    if(giveMeTheRound(gameState) > 0){
        printf("I am at %s\n", idToName(whereIs(gameState, PLAYER_DRACULA)));
    }
    int a;
    int *locs = whereCanIgo(gameState, &a, 1,1);
    int q = 0;
    while (q<a){
        printf ("%s, ", idToName(locs[q]));
        q++;
    }
    printf("\n");

    int trail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
    q = 0;
    printf("trail is...");
    while (q<TRAIL_SIZE){
        printf ("%s, ", idToName(trail[q]));
        q++;
        if(trail[q] == -1) break;
    }
    printf("\n");
    return;
    //..............................................................  
}

int inShortestPath(int move,  DracView dv){
    if(giveMeTheRound(dv)== 0) return TRUE;
    PlayerID startLoc[NUM_PLAYERS-1];
    int length;
    int player = PLAYER_LORD_GODALMING;
    while(player<NUM_PLAYERS-1){
        int *path = minPathFinder(dv,whereIs(dv, PLAYER_DRACULA), whereIs(dv, player), &length);
        startLoc[player] = path[1];
        player++;
    }
    int j = 0;
    while(j<NUM_PLAYERS-1){
        printf("%s|", idToName(startLoc[j]));
        j++;
    }
    printf("\n");

    j=0;
    while(j<NUM_PLAYERS-1){
        if(startLoc[j] == move){
            return TRUE;
        }
        j++;
    }
    return FALSE;
}

int findMaximum(int *a, int n) {
    
    int i;
    int found = FALSE;
    int max;
    int move = 0;
    int dontChoose = INT_MAX;
    //printf("starts\n");
    while(found == FALSE){
        max = a[0];
        move = 0;
        i = 1;
        //printf("1i =  %d\n", i);
        while (i < n){
            if (a[i] >= max && i != dontChoose){
               move = i;
               max = a[i];
            }
            i++;
        
        }if(move != ST_JOSEPH_AND_ST_MARYS && idToType(move) != SEA){
            found = TRUE;
            break;  
        }else{
            dontChoose = move;
        }
        //printf("i =  %d\n", i);
        //printf("move is %s\n", idToName(move));
        //printf("nsh %d\n",dontChoose);
    }
    

    if(found == TRUE){
        return move;
    }

    return NOT_FOUND;
}

int chooseFurtherst(DracView dv){
    int lengths[NUM_MAP_LOCATIONS];
    int loc = 0;
    int length1=0;
    int length2=0;
    int length3=0;
    int length4=0;
    while(loc<NUM_MAP_LOCATIONS){
        length1=0;
        length2=0;
        length3=0;
        length4=0;
        //printf("loc %d\n",loc);
        minPathFinder(dv,loc, whereIs(dv, PLAYER_LORD_GODALMING), &length1);
        //printf("length1 = %d\n", length1);
        minPathFinder(dv,loc, whereIs(dv, PLAYER_DR_SEWARD), &length2);
        //printf("length2 = %d\n", length2);
        minPathFinder(dv,loc, whereIs(dv, PLAYER_VAN_HELSING), &length3);
        //printf("length3 = %d\n", length3);
        minPathFinder(dv,loc, whereIs(dv, PLAYER_MINA_HARKER), &length4);
        //printf("length4 = %d\n", length4);
        //printf("minimum = %d\n", minimum(length1, length2, length3,length4));
        lengths[loc] = minimum(length1, length2, length3,length4);
        loc++;
    }  

    //int poo = 0;
    //loc = 0;
    //while(poo<NUM_MAP_LOCATIONS){
    //    printf("|| %d -> %d ||" ,poo,lengths[poo]);
    //    poo++;
    //}
    //printf("\n"); 

    int bestLoc = findMaximum(lengths, NUM_MAP_LOCATIONS);
    if (bestLoc == NOT_FOUND){
        return CASTLE_DRACULA;
    }
    //minimum length from all players?
    //find legal max(using function above)
    //return loc
    return bestLoc;
} 

static int minimum(int num1, int num2, int num3, int num4){ 
    int a[4] = {num1, num2, num3, num4};
    int c; 
    int min; 

    min = a[0];
    c = 1;
    while(c<4) {
        if (a[c] < min) {
           min = a[c];
        }
        c++;
    }
    return min;
} 
        