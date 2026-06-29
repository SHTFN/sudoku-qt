#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include "sudokuboard.h"

struct SolverCell
{
	int value = 0;
	QSet< int > candidates;
};

struct CandidateAction
{
	int row, col, value;
};

enum SolverResult
{
	SUDOKU_SOLVER_NO_MOVES,
	SUDOKU_SOLVER_HIDDEN_SINGLES,
	SUDOKU_SOLVER_NAKED_PAIRS,
	SUDOKU_SOLVER_HIDDEN_PAIRS,
	SUDOKU_SOLVER_POINTING_STARS,
	SUDOKU_SOLVER_BOX_LINE_REDUCTION,
	SUDOKU_SOLVER_NAKED_TRIPLES,
	SUDOKU_SOLVER_HIDDEN_TRIPLES
};

struct Hint
{
	SolverResult strategy = SUDOKU_SOLVER_NO_MOVES;
	QVector< CandidateAction > setValues;
	QVector< CandidateAction > removeCandidates;
};

using SolverBoard = QVector< QVector< SolverCell > >;

class SudokuSolver
{
  public:
	static Hint tryToSolve(const SudokuBoard &board);

  private:
	static SolverBoard buildSolverBoard(const SudokuBoard &board);
	static QSet< int > calculateCandidates(const SudokuBoard &board, int row, int col);

	static bool hiddenSingles(SolverBoard &board, Hint &hint);
	static bool nakedPairs(SolverBoard &board, Hint &hint);
	static bool hiddenPairs(SolverBoard &board, Hint &hint);
	static bool pointingStars(SolverBoard &board, Hint &hint);
	static bool boxLineReduction(SolverBoard &board, Hint &hint);
	static bool nakedTriples(SolverBoard &board, Hint &hint);
	static bool hiddenTriples(SolverBoard &board, Hint &hint);

	static bool hiddenSinglesRows(SolverBoard &board, Hint &hint);
	static bool hiddenSinglesCols(SolverBoard &board, Hint &hint);
	static bool hiddenSinglesBlocks(SolverBoard &board, Hint &hint);

	static bool nakedPairsRows(SolverBoard &board, Hint &hint);
	static bool nakedPairsCols(SolverBoard &board, Hint &hint);
	static bool nakedPairsBlocks(SolverBoard &board, Hint &hint);

	static bool hiddenPairsRows(SolverBoard &board, Hint &hint);
	static bool hiddenPairsCols(SolverBoard &board, Hint &hint);
	static bool hiddenPairsBlocks(SolverBoard &board, Hint &hint);

	static bool pointingStarsRows(SolverBoard &board, Hint &hint, int startRow, int startCol, int value);
	static bool pointingStarsCols(SolverBoard &board, Hint &hint, int startRow, int startCol, int value);
	static bool pointingStarsBlocks(SolverBoard &board, Hint &hint);

	static bool boxLineReductionRows(SolverBoard &board, Hint &hint);
	static bool boxLineReductionCols(SolverBoard &board, Hint &hint);

	static bool nakedTriplesRows(SolverBoard &board, Hint &hint);
	static bool nakedTriplesCols(SolverBoard &board, Hint &hint);
	static bool nakedTriplesBlocks(SolverBoard &board, Hint &hint);

	static bool hiddenTriplesRows(SolverBoard &board, Hint &hint);
	static bool hiddenTriplesCols(SolverBoard &board, Hint &hint);
	static bool hiddenTriplesBlocks(SolverBoard &board, Hint &hint);
};

#endif	  // SUDOKUSOLVER_H
