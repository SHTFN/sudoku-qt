#ifndef SUDOKUGAMEMANAGER_H
#define SUDOKUGAMEMANAGER_H

#include "sudokuboard.h"
#include "sudokuerrors.h"
#include "sudokufilemanager.h"
#include "sudokusolver.h"
#include "sudokutimer.h"
#include "sudokuvalidator.h"

class SudokuGameManager
{
  public:
	SudokuBoard board;
	SudokuTimer timer;

	bool gameStarted = false;
	bool gameFinished = false;

	bool finishGame();

	void startGame();
	void newGame();
	bool setValue(int row, int col, int value);
	bool clearValue(int row, int col);
	bool undo();
	bool redo();
	Hint makeSolverStep();
	bool isSolved() const;
};

#endif	  // SUDOKUGAMEMANAGER_H
