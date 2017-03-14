#include "strategy.h"
#import "math.h"

void Strategy::applyMoveToBlobs(const movement& mv, bidiarray<Sint16> &blobs, Uint16 player) {
    if ( abs(mv.nx-mv.ox)==2 || abs(mv.ny - mv.oy)==2 ) {
        *blobs.set(mv.ox, mv.oy, -1);
    }
    *blobs.set(mv.nx, mv.ny, player);
}

void Strategy::applyMove (const movement& mv) {
        // To be completed...
}

Sint32 Strategy::estimateCurrentScore () const {
        // To be completed...
    return 0;
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


Sint32 Strategy::computeMyMove (int remainingDepth, bidiarray<Sint16> blobs) {
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
}

void Strategy::computeBestMove () {


    // To be improved...

    //The following code find a valid move.
    /*
    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }*/
    }

end_choice:
     _saveBestMove(mv);
     return;
}
