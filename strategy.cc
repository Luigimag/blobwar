#include "strategy.h"
#include <math.h>

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

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves, Sint16 player) const {
        for(int xblob = 0 ; xblob < 8 ; xblob++) {
            for(int yblob = 0 ; yblob < 8 ; yblob++) {
                if (_blobs.get(xblob, yblob) == (int) player) {
                    //iterate on possible destinations
                    for(int xpos = std::max(0,xblob-2) ; xpos <= std::min(7,xblob+2) ; xpos++) {
                        for(int ypos = std::max(0,yblob-2) ; ypos <= std::min(7,yblob+2) ; ypos++) {
                            if (_holes.get(xpos, ypos)) continue;
                            if (_blobs.get(xpos, ypos) == -1) {
                                movement* mv = new movement(xblob,yblob,xpos,ypos);
				valid_moves.resize(valid_moves.size()+1);
                                valid_moves[valid_moves.size()-1]=*mv;
                            };
                        }
                    }
                }
            }
        }
    return valid_moves;
}


Sint32 Strategy::computeMyMove (int remainingDepth, bidiarray<Sint16> blobs, Sint32 limit) {
    cout<<"Entering computeMyMove\n";
    vector<movement>* valid_moves=new vector<movement>();
    *valid_moves=computeValidMoves(*valid_moves,_current_player);
    Sint32 currentMax=-64;
    Sint32 current=0;
    movement currentBestMove(0,0,0,0);
    for (auto it = valid_moves->begin(); it != valid_moves->end(); ++it) {
	cout<<"MyMove : ";
	cout << it->ox << it->oy << it->nx << it->ny;
        if (remainingDepth == 0) {
            current=estimateCurrentScore(blobs);
            if (current>currentMax) {
                currentMax=current;
                currentBestMove=*it;
            }
        }
        else {
            bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(blobs);
            applyMoveToBlobs(*it,*newBlobs,(int) _current_player);
            current=computeYourMove(remainingDepth-1,*newBlobs,currentMax);
	    
	    if (current>=limit) {
		cout<<"current>limit";
		currentMax = current;
		break;
	    }
            if (current>currentMax) {
                currentMax=current;
                currentBestMove=*it;
            }
        }
    }
    return currentMax;
}

Sint32 Strategy::computeYourMove(int remainingDepth, bidiarray<Sint16> blobs, Sint32 limit) {
    cout<<"Entering computeYourMove\n";
    vector<movement>* valid_moves=new vector<movement>();
    *valid_moves=computeValidMoves(*valid_moves,!_current_player);
    Sint32 currentMin=64;
    Sint32 current=0;
    movement currentBestMove(0,0,0,0);
    for (auto it = valid_moves->begin(); it != valid_moves->end(); ++it) {
	cout<<"YourMove : ";
	cout << it->ox << it->oy << it->nx << it->ny;
        if (remainingDepth == 0) {
            current=estimateCurrentScore(blobs);
            if (current<currentMin) {
                currentMin=current;
                currentBestMove=*it;
            }
        }
        else {
            bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(blobs);
            applyMoveToBlobs(*it,*newBlobs,(int) !_current_player);
            current=computeMyMove(remainingDepth-1,*newBlobs,currentMin);
	    if (current <= limit) {
		cout<<"current <= limit";
		currentMin = current;
		break;
	    }
            if (current<currentMin) {
                currentMin=current;
                currentBestMove=*it;
            }
        }
    }
    return currentMin;
}

void Strategy::computeBestMove () {
    for (int i = 1; i < 50; i++) {
	vector<movement>* valid_moves=new vector<movement>();
	*valid_moves=computeValidMoves(*valid_moves,_current_player);
	Sint32 currentMax=-64;
	Sint32 current=0;
	movement currentBestMove(0,0,0,0);
	for (auto it = valid_moves->begin(); it != valid_moves->end(); ++it) {
	    cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	    bidiarray<Sint16>* newBlobs=new bidiarray<Sint16>(_blobs);
	    applyMoveToBlobs(*it,*newBlobs,(int) _current_player);
	    current=computeYourMove(i-1,*newBlobs,currentMax);
	    if (current>currentMax) {
		currentMax=current;
		currentBestMove=*it;
	    }
	}
	_saveBestMove(currentBestMove);
    }
    return;
}
