#include "strategy.h"
#include <math.h>
#include <omp.h>
void Strategy::applyMoveToBlobs(const movement& mv, bidiarray<Sint16> &blobs, Uint16 player) {
    if ( abs(mv.nx-mv.ox)==2 || abs(mv.ny - mv.oy)==2 ) {
	blobs.set(mv.ox, mv.oy, -1);
    }
    blobs.set(mv.nx, mv.ny, player);

    for(int xpos = std::max(0,mv.nx-1) ; xpos <= std::min(7,mv.nx+1) ; xpos++) {
	for(int ypos = std::max(0,mv.ny-1) ; ypos <= std::min(7,mv.ny+1) ; ypos++) {
	    if (blobs.get(xpos,ypos)==!(player)) {
		blobs.set(xpos,ypos,player);
	    }
	}
    }
}

void Strategy::applyMove (const movement& mv) {
    applyMoveToBlobs(mv,_blobs,_current_player);
}

Sint32 Strategy::estimateCurrentScore (bidiarray<Sint16> blobs) const {
    //Score = number of allied blobs - number of enemy blobs
    int score=0;
    for (int iteX = 0; iteX <8 ; iteX++) {
	for (int iteY = 0; iteY<8 ; iteY++) {
	    if (blobs.get(iteX,iteY) == 0 || blobs.get(iteX,iteY) == 1) {
		if (blobs.get(iteX, iteY) == (int) _current_player) {
		    score++;
		}
		else {
		    score--;
		}
	    }
	}
    }
    return score;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves, bidiarray<Sint16> blobs, Sint16 player) const {
    for(int xblob = 0 ; xblob < 8 ; xblob++) {
	for(int yblob = 0 ; yblob < 8 ; yblob++) {
	    if (blobs.get(xblob, yblob) == (int) player) {
		//iterate on possible destinations
		for(int xpos = std::max(0,xblob-1) ; xpos <= std::min(7,xblob+1) ; xpos++) {
		    for(int ypos = std::max(0,yblob-1) ; ypos <= std::min(7,yblob+1) ; ypos++) {
			if (_holes.get(xpos, ypos)) continue;
			if (blobs.get(xpos, ypos) == -1) {
			    movement* mv = new movement(xblob,yblob,xpos,ypos);
			    valid_moves.resize(valid_moves.size()+1);
			    valid_moves[valid_moves.size()-1]=*mv;
			}
		    }
		}

		for(int xpos = std::max(0,xblob-2) ; xpos <= std::min(7,xblob+2) ; xpos++) {
		    for(int ypos = std::max(0,yblob-2) ; ypos <= std::min(7,yblob+2) ; ypos++) {
			if (_holes.get(xpos, ypos)) continue;
			if ( abs(xpos-xblob)==2 || abs(ypos - yblob)==2 ) {
			    if (blobs.get(xpos, ypos) == -1) {
				movement* mv = new movement(xblob,yblob,xpos,ypos);
				valid_moves.resize(valid_moves.size()+1);
				valid_moves[valid_moves.size()-1]=*mv;
			    }
			}
		    }
		}
	    }
	}
    }
    return valid_moves;
}


Sint32 Strategy::computeMyMove (int remainingDepth, bidiarray<Sint16> blobs, Sint32 limit) {
    vector<movement>* valid_moves=new vector<movement>();
    *valid_moves=computeValidMoves(*valid_moves,blobs,_current_player);
    if (valid_moves->empty()) {
	return estimateCurrentScore(blobs);
    }
    Sint32 currentMax=-64; //Minimum possible value
    Sint32 currentScore=0;
    for (auto it = valid_moves->begin(); it != valid_moves->end(); ++it) {
	//Clones blobs to build the alternate situation on which we will work in the incoming iterations
	bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(blobs);
	applyMoveToBlobs(*it,*newBlobs,(int) _current_player);
	if (remainingDepth == 0) {
	    currentScore=estimateCurrentScore(*newBlobs);
	    if (currentScore>currentMax) {
		currentMax=currentScore;
	    }
	}
	else {
	    currentScore=computeYourMove(remainingDepth-1,*newBlobs,currentMax);
	    if (currentScore>currentMax) {
		currentMax=currentScore;
	    }
	}
	if (currentScore>=limit) {
	    //Condition for exploration cut.
	    currentMax = currentScore;
	    break;
	}

	delete newBlobs;
    }
    delete valid_moves;
    return currentMax;
}

Sint32 Strategy::computeYourMove(int remainingDepth, bidiarray<Sint16> blobs, Sint32 limit) {
    vector<movement>* valid_moves=new vector<movement>();
    *valid_moves=computeValidMoves(*valid_moves,blobs,!_current_player);
    if (valid_moves->empty()) {
	return estimateCurrentScore(blobs);
    }
    Sint32 currentMin=64; //Maximum possible value
    Sint32 currentScore=0;
    for (auto it = valid_moves->begin(); it != valid_moves->end(); ++it) {
	//clones blobs to build the alternate situation on which we will work in the incoming iterations
	bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(blobs);
	applyMoveToBlobs(*it,*newBlobs,(int) !_current_player);
	if (remainingDepth == 0) {
	    currentScore=estimateCurrentScore(*newBlobs);
	    if (currentScore<currentMin) {
		currentMin=currentScore;
	    }
	}
	else {
	    currentScore=computeMyMove(remainingDepth-1,*newBlobs,currentMin);
	    if (currentScore<currentMin) {
		currentMin=currentScore;
	    }
	}
	if (currentScore <= limit) {
	    //Condition for exploration cut.
	    currentMin = currentScore;
	    break;
	}

	delete newBlobs;
    }
    delete valid_moves;
    return currentMin;
}

void Strategy::computeBestMove () {
    for (int i = 1; i < 50; i++) {
	vector<movement>* valid_moves=new vector<movement>();
	*valid_moves=computeValidMoves(*valid_moves,_blobs,_current_player);
	Sint32 currentMax=-65; // < -64, so that (0,0,0,0) always gets overriden
	Sint32 currentScore=0;
	movement currentBestMove(0,0,0,0);
	#pragma omp parallel for
	for (auto it = valid_moves->begin(); it < valid_moves->end(); ++it) {
	    bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(_blobs);
	    applyMoveToBlobs(*it,*newBlobs,(int) _current_player);
	    currentScore=computeYourMove(i-1,*newBlobs,currentMax);
	    if (currentScore>currentMax) {
		currentMax=currentScore;
		currentBestMove=*it;
	    }
	    delete newBlobs;
	}
	_saveBestMove(currentBestMove);
	delete valid_moves;
    }
    return;
}
