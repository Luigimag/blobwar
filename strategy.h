#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "common.h"
#include "bidiarray.h"
#include "movement.h"
#include <vector>



class Strategy {

 private:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;

    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(movement&);

 public:
    // Constructor from a current situation
    Strategy (bidiarray<Sint16>& blobs,
	      const bidiarray<bool>& holes,
	      const Uint16 current_player,
              void (*saveBestMove)(movement&))
	: _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove)
    {
    }



    // Copy constructor
    Strategy (const Strategy& St)
	: _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player)
        {}

    // Destructor
    ~Strategy() {}

    /**
     * Applies a move to the state of blobs in parameters, not the blobs in strategy
     */
    void applyMoveToBlobs(const movement& mv, bidiarray<Sint16> &blobs, Uint16 player);

    /**
     *Apply a move to the current state of blobs
     * Assumes that the move is valid
     */
    void applyMove (const movement& mv);

    /**
     * Compute the vector containing every possible moves
     */
    vector<movement>& computeValidMoves (vector<movement>& valid_moves, bidiarray<Sint16> blobs, Sint16 player) const;

    /**
     * Estimate the score of the current state of the game
     */
    float estimateCurrentScore (bidiarray<Sint16> blobs) const;

    /**
     * Max layers of the MinMax algorithm
     * blobs : the position of blobs in the current iteration
     * limit : the lowest score that rules the node out
     */
    float computeMyMove(int remainingDepth, bidiarray<Sint16> blobs, float limit);

    /**
     * Min layers of the MinMax algorithm
     * blobs : the position of blobs in the current iteration
     * limit : the highest score that rules the node out
     */
    float computeYourMove(int remainingDepth, bidiarray<Sint16> blobs, float limit);

    /**
     * Finds the best move.
     * Initializer of the recursive process.
     * Therefore, behaves similarly to computeMyMove, but saves the move at the end.
     */
    void computeBestMove ();

};

#endif
