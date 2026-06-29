#include "sudokusolver.h"

#include <QDebug>
#include <QSet>
#include <QVector>

Hint SudokuSolver::tryToSolve(const SudokuBoard &board)
{
	Hint hint;
	hint.strategy = SUDOKU_SOLVER_NO_MOVES;

	SolverBoard solverBoard = buildSolverBoard(board);

	if (hiddenSingles(solverBoard, hint))
		return hint;
	if (nakedPairs(solverBoard, hint))
		return hint;
	if (hiddenPairs(solverBoard, hint))
		return hint;
	if (pointingStars(solverBoard, hint))
		return hint;
	if (boxLineReduction(solverBoard, hint))
		return hint;
	if (nakedTriples(solverBoard, hint))
		return hint;
	if (hiddenTriples(solverBoard, hint))
		return hint;

	return hint;
}

//-------------------------------------------------------------------------------------------------------------------

SolverBoard SudokuSolver::buildSolverBoard(const SudokuBoard &board)
{
	SolverBoard solverBoard(SudokuBoard::BOARD_SIZE, QVector< SolverCell >(SudokuBoard::BOARD_SIZE));

	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			solverBoard[row][col].value = board.getValue(row, col);

			if (solverBoard[row][col].value == 0)
			{
				solverBoard[row][col].candidates = calculateCandidates(board, row, col);
			}
		}
	}

	return solverBoard;
}

QSet< int > SudokuSolver::calculateCandidates(const SudokuBoard &board, int row, int col)
{
	QSet< int > candidates;

	for (int num = 1; num <= SudokuBoard::BOARD_SIZE; num++)
	{
		if (board.isValidPlacement(row, col, num))
			candidates.insert(num);
	}

	return candidates;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::hiddenSingles(SolverBoard &board, Hint &hint)
{
	return hiddenSinglesRows(board, hint) || hiddenSinglesCols(board, hint) || hiddenSinglesBlocks(board, hint);
}

bool SudokuSolver::nakedPairs(SolverBoard &board, Hint &hint)
{
	return nakedPairsRows(board, hint) || nakedPairsCols(board, hint) || nakedPairsBlocks(board, hint);
}

bool SudokuSolver::hiddenPairs(SolverBoard &board, Hint &hint)
{
	return hiddenPairsRows(board, hint) || hiddenPairsCols(board, hint) || hiddenPairsBlocks(board, hint);
}

bool SudokuSolver::pointingStars(SolverBoard &board, Hint &hint)
{
	return pointingStarsBlocks(board, hint);
}

bool SudokuSolver::boxLineReduction(SolverBoard &board, Hint &hint)
{
	return boxLineReductionRows(board, hint) || boxLineReductionCols(board, hint);
}

bool SudokuSolver::nakedTriples(SolverBoard &board, Hint &hint)
{
	return nakedTriplesRows(board, hint) || nakedTriplesCols(board, hint) || nakedTriplesBlocks(board, hint);
}

bool SudokuSolver::hiddenTriples(SolverBoard &board, Hint &hint)
{
	return hiddenTriplesRows(board, hint) || hiddenTriplesCols(board, hint) || hiddenTriplesBlocks(board, hint);
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::hiddenPairsRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 1; value1++)
		{
			for (int value2 = 1; value2 <= SudokuBoard::BOARD_SIZE; value2++)
			{
				QVector< int > cols1;
				QVector< int > cols2;

				for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
				{
					if (board[row][col].candidates.contains(value1))
						cols1.append(col);

					if (board[row][col].candidates.contains(value2))
						cols2.append(col);
				}

				if (cols1.size() != 2 || cols2.size() != 2)
					continue;

				if (cols1 != cols2)
					continue;

				bool removed = false;

				for (int col : cols1)
				{
					QSet< int > candidates = board[row][col].candidates;

					for (int candidate : candidates)
					{
						if (candidate != value1 && candidate != value2)
						{
							hint.removeCandidates.append({ row, col, candidate });
							removed = true;
						}
					}
				}

				if (removed)
				{
					hint.strategy = SUDOKU_SOLVER_HIDDEN_PAIRS;
					return true;
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenPairsCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 1; value1++)
		{
			for (int value2 = 1; value2 <= SudokuBoard::BOARD_SIZE; value2++)
			{
				QVector< int > rows1;
				QVector< int > rows2;

				for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
				{
					if (board[row][col].candidates.contains(value1))
						rows1.append(row);

					if (board[row][col].candidates.contains(value2))
						rows2.append(row);
				}

				if (rows1.size() != 2 || rows2.size() != 2)
					continue;

				if (rows1 != rows2)
					continue;

				bool removed = false;

				for (int row : rows1)
				{
					QSet< int > candidates = board[row][col].candidates;

					for (int candidate : candidates)
					{
						if (candidate != value1 && candidate != value2)
						{
							hint.removeCandidates.append({ row, col, candidate });
							removed = true;
						}
					}
				}

				if (removed)
				{
					hint.strategy = SUDOKU_SOLVER_HIDDEN_PAIRS;
					return true;
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenPairsBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 1; value1++)
			{
				for (int value2 = value1 + 1; value2 <= SudokuBoard::BOARD_SIZE; value2++)
				{
					QVector< QPair< int, int > > cells1;
					QVector< QPair< int, int > > cells2;

					for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
					{
						for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
						{
							if (board[row][col].value != 0)
							{
								continue;
							}

							if (board[row][col].candidates.contains(value1))
							{
								cells1.append({ row, col });
							}

							if (board[row][col].candidates.contains(value2))
							{
								cells2.append({ row, col });
							}
						}
					}

					if (cells1.size() != 2 || cells2.size() != 2)
					{
						continue;
					}

					if (cells1 != cells2)
					{
						continue;
					}

					bool removed = false;

					for (auto cell : cells1)
					{
						int row = cell.first;
						int col = cell.second;

						QSet< int > candidates = board[row][col].candidates;

						for (int candidate : candidates)
						{
							if (candidate != value1 && candidate != value2)
							{
								hint.removeCandidates.append({ row, col, candidate });

								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_HIDDEN_PAIRS;
						return true;
					}
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::nakedPairsRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col1 = 0; col1 < SudokuBoard::BOARD_SIZE - 1; col1++)
		{
			QSet< int > candidates1 = board[row][col1].candidates;

			if (candidates1.size() != 2)
				continue;

			for (int col2 = col1 + 1; col2 < SudokuBoard::BOARD_SIZE; col2++)
			{
				QSet< int > candidates2 = board[row][col2].candidates;

				if (candidates1 != candidates2)
					continue;

				bool removed = false;

				for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
				{
					if (col == col1 || col == col2)
						continue;

					for (int value : candidates1)
					{
						if (board[row][col].candidates.contains(value))
						{
							hint.removeCandidates.append({ row, col, value });
							removed = true;
						}
					}
				}

				if (removed)
				{
					hint.strategy = SUDOKU_SOLVER_NAKED_PAIRS;

					return true;
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::nakedPairsCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int row1 = 0; row1 < SudokuBoard::BOARD_SIZE - 1; row1++)
		{
			QSet< int > candidates1 = board[row1][col].candidates;

			if (candidates1.size() != 2)
				continue;

			for (int row2 = row1 + 1; row2 < SudokuBoard::BOARD_SIZE; row2++)
			{
				QSet< int > candidates2 = board[row2][col].candidates;

				if (candidates1 != candidates2)
					continue;

				bool removed = false;
				for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
				{
					if (row == row1 || row == row2)
						continue;

					for (int value : candidates1)
					{
						if (board[row][col].candidates.contains(value))
						{
							hint.removeCandidates.append({ row, col, value });
							removed = true;
						}
					}
				}

				if (removed)
				{
					hint.strategy = SUDOKU_SOLVER_NAKED_PAIRS;

					return true;
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::nakedPairsBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			QVector< QPair< int, int > > cells;

			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
			{
				for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
				{
					cells.append({ row, col });
				}
			}

			for (int i = 0; i < cells.size() - 1; i++)
			{
				int row1 = cells[i].first;
				int col1 = cells[i].second;

				QSet< int > pair1 = board[row1][col1].candidates;

				if (pair1.size() != 2)
				{
					continue;
				}

				for (int j = i + 1; j < cells.size(); j++)
				{
					int row2 = cells[j].first;
					int col2 = cells[j].second;

					QSet< int > pair2 = board[row2][col2].candidates;

					if (pair1 != pair2)
					{
						continue;
					}

					bool removed = false;

					for (auto cell : cells)
					{
						int row = cell.first;
						int col = cell.second;

						if ((row == row1 && col == col1) || (row == row2 && col == col2))
						{
							continue;
						}

						for (int value : pair1)
						{
							if (board[row][col].candidates.contains(value))
							{
								hint.removeCandidates.append({ row, col, value });
								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_NAKED_PAIRS;
						return true;
					}
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::hiddenSinglesRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
		{
			int count = 0;
			int foundCol = -1;

			for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
			{
				if (board[row][col].value != 0)
					continue;

				if (board[row][col].candidates.contains(value))
				{
					count++;
					foundCol = col;
				}
			}

			if (count == 1)
			{
				hint.strategy = SUDOKU_SOLVER_HIDDEN_SINGLES;
				hint.setValues.append({ row, foundCol, value });

				return true;
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenSinglesCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
		{
			int count = 0;
			int foundRow = -1;

			for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
			{
				if (board[row][col].value != 0)
					continue;

				if (board[row][col].candidates.contains(value))
				{
					count++;
					foundRow = row;
				}
			}

			if (count == 1)
			{
				hint.strategy = SUDOKU_SOLVER_HIDDEN_SINGLES;
				hint.setValues.append({ foundRow, col, value });

				return true;
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenSinglesBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
			{
				int count = 0;

				int foundRow = -1;
				int foundCol = -1;

				for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
				{
					for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
					{
						if (board[row][col].value != 0)
							continue;

						if (board[row][col].candidates.contains(value))
						{
							count++;

							foundRow = row;
							foundCol = col;
						}
					}
				}

				if (count == 1)
				{
					hint.strategy = SUDOKU_SOLVER_HIDDEN_SINGLES;
					hint.setValues.append({ foundRow, foundCol, value });

					return true;
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::pointingStarsRows(SolverBoard &board, Hint &hint, int startRow, int startCol, int value)
{
	int foundRow = -1;
	int count = 0;

	for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
	{
		for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
		{
			if (board[row][col].value != 0)
				continue;

			if (!board[row][col].candidates.contains(value))
				continue;

			if (foundRow == -1)
				foundRow = row;

			if (foundRow != row)
				return false;

			count++;
		}
	}

	if (count < 2)
		return false;

	bool removed = false;

	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		if (col >= startCol && col < startCol + SudokuBoard::BLOCK_SIZE)
			continue;

		if (board[foundRow][col].value == 0 && board[foundRow][col].candidates.contains(value))
		{
			hint.removeCandidates.append({ foundRow, col, value });
			removed = true;
		}
	}

	if (removed)
		hint.strategy = SUDOKU_SOLVER_POINTING_STARS;

	return removed;
}

bool SudokuSolver::pointingStarsCols(SolverBoard &board, Hint &hint, int startRow, int startCol, int value)
{
	int foundCol = -1;
	int count = 0;

	for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
	{
		for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
		{
			if (board[row][col].value != 0)
				continue;

			if (!board[row][col].candidates.contains(value))
				continue;

			if (foundCol == -1)
				foundCol = col;

			if (foundCol != col)
				return false;

			count++;
		}
	}

	if (count < 2)
		return false;

	bool removed = false;

	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		if (row >= startRow && row < startRow + SudokuBoard::BLOCK_SIZE)
			continue;

		if (board[row][foundCol].value == 0 && board[row][foundCol].candidates.contains(value))
		{
			hint.removeCandidates.append({ row, foundCol, value });
			removed = true;
		}
	}

	if (removed)
		hint.strategy = SUDOKU_SOLVER_POINTING_STARS;

	return removed;
}

bool SudokuSolver::pointingStarsBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
			{
				if (pointingStarsRows(board, hint, startRow, startCol, value))
					return true;

				if (pointingStarsCols(board, hint, startRow, startCol, value))
					return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::boxLineReductionRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
		{
			int foundBlockCol = -1;
			int count = 0;

			for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
			{
				if (board[row][col].value != 0)
					continue;

				if (!board[row][col].candidates.contains(value))
					continue;

				int blockCol = col / SudokuBoard::BLOCK_SIZE;

				if (foundBlockCol == -1)
					foundBlockCol = blockCol;

				if (foundBlockCol != blockCol)
					count = -1;

				if (count == -1)
					break;

				count++;
			}

			if (count < 2)
				continue;

			int startRow = (row / SudokuBoard::BLOCK_SIZE) * SudokuBoard::BLOCK_SIZE;
			int startCol = foundBlockCol * SudokuBoard::BLOCK_SIZE;

			bool removed = false;

			for (int r = startRow; r < startRow + SudokuBoard::BLOCK_SIZE; r++)
			{
				if (r == row)
					continue;

				for (int c = startCol; c < startCol + SudokuBoard::BLOCK_SIZE; c++)
				{
					if (board[r][c].value != 0)
						continue;

					if (board[r][c].candidates.contains(value))
					{
						hint.removeCandidates.append({ r, c, value });
						removed = true;
					}
				}
			}

			if (removed)
			{
				hint.strategy = SUDOKU_SOLVER_BOX_LINE_REDUCTION;
				return true;
			}
		}
	}

	return false;
}

bool SudokuSolver::boxLineReductionCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int value = 1; value <= SudokuBoard::BOARD_SIZE; value++)
		{
			int foundBlockRow = -1;
			int count = 0;

			for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
			{
				if (board[row][col].value != 0)
					continue;

				if (!board[row][col].candidates.contains(value))
					continue;

				int blockRow = row / SudokuBoard::BLOCK_SIZE;

				if (foundBlockRow == -1)
					foundBlockRow = blockRow;

				if (foundBlockRow != blockRow)
					count = -1;

				if (count == -1)
					break;

				count++;
			}

			if (count < 2)
				continue;

			int startRow = foundBlockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = (col / SudokuBoard::BLOCK_SIZE) * SudokuBoard::BLOCK_SIZE;

			bool removed = false;

			for (int r = startRow; r < startRow + SudokuBoard::BLOCK_SIZE; r++)
			{
				for (int c = startCol; c < startCol + SudokuBoard::BLOCK_SIZE; c++)
				{
					if (c == col)
						continue;

					if (board[r][c].value != 0)
						continue;

					if (board[r][c].candidates.contains(value))
					{
						hint.removeCandidates.append({ r, c, value });
						removed = true;
					}
				}
			}

			if (removed)
			{
				hint.strategy = SUDOKU_SOLVER_BOX_LINE_REDUCTION;
				return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::nakedTriplesRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col1 = 0; col1 < SudokuBoard::BOARD_SIZE - 2; col1++)
		{
			QSet< int > candidates1 = board[row][col1].candidates;

			if (candidates1.size() < 2 || candidates1.size() > 3)
				continue;

			for (int col2 = col1 + 1; col2 < SudokuBoard::BOARD_SIZE - 1; col2++)
			{
				QSet< int > candidates2 = board[row][col2].candidates;

				if (candidates2.size() < 2 || candidates2.size() > 3)
					continue;

				for (int col3 = col2 + 1; col3 < SudokuBoard::BOARD_SIZE; col3++)
				{
					QSet< int > candidates3 = board[row][col3].candidates;

					if (candidates3.size() < 2 || candidates3.size() > 3)
						continue;

					QSet< int > triple = candidates1;
					triple.unite(candidates2);
					triple.unite(candidates3);

					if (triple.size() != 3)
						continue;

					bool removed = false;

					for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
					{
						if (col == col1 || col == col2 || col == col3)
							continue;

						for (int value : triple)
						{
							if (board[row][col].candidates.contains(value))
							{
								hint.removeCandidates.append({ row, col, value });
								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_NAKED_TRIPLES;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::nakedTriplesCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int row1 = 0; row1 < SudokuBoard::BOARD_SIZE - 2; row1++)
		{
			QSet< int > candidates1 = board[row1][col].candidates;

			if (candidates1.size() < 2 || candidates1.size() > 3)
				continue;

			for (int row2 = row1 + 1; row2 < SudokuBoard::BOARD_SIZE - 1; row2++)
			{
				QSet< int > candidates2 = board[row2][col].candidates;

				if (candidates2.size() < 2 || candidates2.size() > 3)
					continue;

				for (int row3 = row2 + 1; row3 < SudokuBoard::BOARD_SIZE; row3++)
				{
					QSet< int > candidates3 = board[row3][col].candidates;

					if (candidates3.size() < 2 || candidates3.size() > 3)
						continue;

					QSet< int > triple = candidates1;
					triple.unite(candidates2);
					triple.unite(candidates3);

					if (triple.size() != 3)
						continue;

					bool removed = false;

					for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
					{
						if (row == row1 || row == row2 || row == row3)
							continue;

						for (int value : triple)
						{
							if (board[row][col].candidates.contains(value))
							{
								hint.removeCandidates.append({ row, col, value });
								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_NAKED_TRIPLES;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::nakedTriplesBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			QVector< QPair< int, int > > cells;

			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
			{
				for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
				{
					cells.append({ row, col });
				}
			}

			for (int i = 0; i < cells.size() - 2; i++)
			{
				int row1 = cells[i].first;
				int col1 = cells[i].second;

				QSet< int > candidates1 = board[row1][col1].candidates;

				if (candidates1.size() < 2 || candidates1.size() > 3)
					continue;

				for (int j = i + 1; j < cells.size() - 1; j++)
				{
					int row2 = cells[j].first;
					int col2 = cells[j].second;

					QSet< int > candidates2 = board[row2][col2].candidates;

					if (candidates2.size() < 2 || candidates2.size() > 3)
						continue;

					for (int k = j + 1; k < cells.size(); k++)
					{
						int row3 = cells[k].first;
						int col3 = cells[k].second;

						QSet< int > candidates3 = board[row3][col3].candidates;

						if (candidates3.size() < 2 || candidates3.size() > 3)
							continue;

						QSet< int > triple = candidates1;
						triple.unite(candidates2);
						triple.unite(candidates3);

						if (triple.size() != 3)
							continue;

						bool removed = false;

						for (auto cell : cells)
						{
							int row = cell.first;
							int col = cell.second;

							if ((row == row1 && col == col1) || (row == row2 && col == col2) || (row == row3 && col == col3))
							{
								continue;
							}

							for (int value : triple)
							{
								if (board[row][col].candidates.contains(value))
								{
									hint.removeCandidates.append({ row, col, value });
									removed = true;
								}
							}
						}

						if (removed)
						{
							hint.strategy = SUDOKU_SOLVER_NAKED_TRIPLES;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------

bool SudokuSolver::hiddenTriplesRows(SolverBoard &board, Hint &hint)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 2; value1++)
		{
			for (int value2 = value1 + 1; value2 <= SudokuBoard::BOARD_SIZE - 1; value2++)
			{
				for (int value3 = value2 + 1; value3 <= SudokuBoard::BOARD_SIZE; value3++)
				{
					QSet< int > cols;

					for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
					{
						if (board[row][col].value != 0)
							continue;

						if (board[row][col].candidates.contains(value1) ||
							board[row][col].candidates.contains(value2) || board[row][col].candidates.contains(value3))
						{
							cols.insert(col);
						}
					}

					if (cols.size() != 3)
						continue;

					bool removed = false;

					for (int col : cols)
					{
						QSet< int > candidates = board[row][col].candidates;

						for (int candidate : candidates)
						{
							if (candidate != value1 && candidate != value2 && candidate != value3)
							{
								hint.removeCandidates.append({ row, col, candidate });
								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_HIDDEN_TRIPLES;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenTriplesCols(SolverBoard &board, Hint &hint)
{
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 2; value1++)
		{
			for (int value2 = value1 + 1; value2 <= SudokuBoard::BOARD_SIZE - 1; value2++)
			{
				for (int value3 = value2 + 1; value3 <= SudokuBoard::BOARD_SIZE; value3++)
				{
					QSet< int > rows;

					for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
					{
						if (board[row][col].value != 0)
							continue;

						if (board[row][col].candidates.contains(value1) ||
							board[row][col].candidates.contains(value2) || board[row][col].candidates.contains(value3))
						{
							rows.insert(row);
						}
					}

					if (rows.size() != 3)
						continue;

					bool removed = false;

					for (int row : rows)
					{
						QSet< int > candidates = board[row][col].candidates;

						for (int candidate : candidates)
						{
							if (candidate != value1 && candidate != value2 && candidate != value3)
							{
								hint.removeCandidates.append({ row, col, candidate });
								removed = true;
							}
						}
					}

					if (removed)
					{
						hint.strategy = SUDOKU_SOLVER_HIDDEN_TRIPLES;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool SudokuSolver::hiddenTriplesBlocks(SolverBoard &board, Hint &hint)
{
	for (int blockRow = 0; blockRow < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockRow++)
	{
		for (int blockCol = 0; blockCol < SudokuBoard::BOARD_SIZE / SudokuBoard::BLOCK_SIZE; blockCol++)
		{
			int startRow = blockRow * SudokuBoard::BLOCK_SIZE;
			int startCol = blockCol * SudokuBoard::BLOCK_SIZE;

			for (int value1 = 1; value1 <= SudokuBoard::BOARD_SIZE - 2; value1++)
			{
				for (int value2 = value1 + 1; value2 <= SudokuBoard::BOARD_SIZE - 1; value2++)
				{
					for (int value3 = value2 + 1; value3 <= SudokuBoard::BOARD_SIZE; value3++)
					{
						QSet< QPair< int, int > > cells;

						for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
						{
							for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
							{
								if (board[row][col].value != 0)
									continue;

								if (board[row][col].candidates.contains(value1) ||
									board[row][col].candidates.contains(value2) || board[row][col].candidates.contains(value3))
								{
									cells.insert({ row, col });
								}
							}
						}

						if (cells.size() != 3)
							continue;

						bool removed = false;

						for (auto cell : cells)
						{
							int row = cell.first;
							int col = cell.second;

							QSet< int > candidates = board[row][col].candidates;

							for (int candidate : candidates)
							{
								if (candidate != value1 && candidate != value2 && candidate != value3)
								{
									hint.removeCandidates.append({ row, col, candidate });
									removed = true;
								}
							}
						}

						if (removed)
						{
							hint.strategy = SUDOKU_SOLVER_HIDDEN_TRIPLES;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}
