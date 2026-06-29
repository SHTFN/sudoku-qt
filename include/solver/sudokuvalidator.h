#ifndef SUDOKUVALIDATOR_H
#define SUDOKUVALIDATOR_H

#include "sudokuboard.h"

class SudokuValidator
{
  public:
	static bool validateBasic(const SudokuBoard *board);
	static bool isSolved(const SudokuBoard *board);

  private:
	static bool checkRows(const SudokuBoard *board);
	static bool checkCols(const SudokuBoard *board);
	static bool checkBlocks(const SudokuBoard *board);

	static bool checkRow(const SudokuBoard *board, int row);
	static bool checkCol(const SudokuBoard *board, int col);
	static bool checkBlock(const SudokuBoard *board, int block);
};

#endif	  // SUDOKUVALIDATOR_H
