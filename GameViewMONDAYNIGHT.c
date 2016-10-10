
void historyGet(GameView currentView, PlayerID player, LocationID trail[TRAIL_SIZE],char *pastPlays){
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
    int z = 0;
	while(z<TRAIL_SIZE){
		trail[z]=currentView->location[player][z];
		z++;
	}
}

