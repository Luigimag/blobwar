#include "strategy.h"
#import "math.h"

void switch0and1(int x){
    if (x==0) {return 1;} else {return 0;}
}

void Strategy::applyMoveToBlobs(const movement& mv, bidiarray<Sint16> &blobs, Uint16 player) {
    if ( abs(mv.nx-mv.ox)==2 || abs(mv.ny - mv.oy)==2 ) {
        *blobs.set(mv.ox, mv.oy, -1);
    }
    *blobs.set(mv.nx, mv.ny, player);

    for(int xpos = std::max(0,mv.nx-1) ; xpos <= std::min(7,mv.nx+1) ; xpos++) {
        for(int ypos = std::max(0,mv.ny-1) ; ypos <= std::min(7,mv.ny+1) ; ypos++) {
            if (*blobs.get(xpos,ypos)==switch0and1(player)) {
                *blobs.set(xpos,ypos,player);
            }
        }
    }
}

void Strategy::applyMove (const movement& mv) {
    applyMoveToBlobs(mv,_blobs,_current_player);

    //else {
    //    _blobs.set(mv.nx,mv.ny,Switch0and1( (int) _current_player );
    //}

}

Sint32 Strategy::estimateCurrentScore (bidiarray<Sint16> blobs) const {
    int score=0;
    for (int iteX = 0; iteX <8 ; iteX++) {
        for (int iteY = 0; iteY<8 ; iteY++) {
            if (blobs.get(iteX,iteY)) == 0 || blobs.get(iteX,iteY)) == 1) {
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

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
        for(int xblob = 0 ; xblob < 8 ; xblob++) {
            for(int yblob = 0 ; yblob < 8 ; yblob++) {
                if (_blobs.get(xblob, yblob) == (int) _current_player) {
                    //iterate on possible destinations
                    for(int xpos = std::max(0,xblob-2) ; xpos <= std::min(7,xblob+2) ; xpos++) {
                        for(int ypos = std::max(0,yblob-2) ; ypos <= std::min(7,yblob+2) ; ypos++) {
                            if (_holes.get(xpos, ypos)) continue;
                            if (_blobs.get(xpos, ypos) == -1) {
                                movement mv(xblob,yblob,xpos,ypos);
                                valid_moves.insert(mv);
                            };
                        }
                    }
                }
            }
        }
    return valid_moves;
}


movement Strategy::computeMyMove (int remainingDepth, bidiarray<Sint16> blobs) {
    vector<movement> valid_moves=new vector<movement>();
    valid_moves=computeValidMoves(valid_moves);
    Sint32 currentMax=-64;
    Sint32 current=0;
    movement currentBestMove(0,0,0,0);
    for (auto it = valid_moves.begin(); it != valid_moves.end(); ++it) {
        if (remainingDepth == 0) {
            current=estimateCurrentScore();
            if (current>currentMax) {
                currentMax=current;
                currentBestMove=*it;
            }
        }
        else {
            bidiarray<Sint16> newBlobs=new bidiarray<Sint16>(blobs);
            applyMoveToBlobs(*it,newBlobs,(int) _current_player);
            current=computeYourMove(remainingDepth-1,newBlobs);
            if (current>currentMax) {
                currentMax=current;
                currentBestMove=*it;
            }
        }
    }
    return currentBestMove;
}

void Strategy::computeBestMove () {
    _saveBestMove( computeMyMove(4,_blobs) );
     return;
}
