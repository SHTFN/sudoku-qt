#include "sudokuboard.h"

#include <QFile>
#include <QString>
#include <QTextStream>

SudokuBoard::SudokuBoard()
{
	board.resize(BOARD_SIZE);
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		board[i].resize(BOARD_SIZE);
	}
}

int SudokuBoard::getValue(int row, int col) const
{
	return board[row][col].value;
}

QSet< int > SudokuBoard::getCandidates(int row, int col) const
{
	return board[row][col].candidates;
}

bool SudokuBoard::removeCandidate(int row, int col, int candidate)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return false;

	if (candidate < 1 || candidate > BOARD_SIZE)
		return false;

	if (board[row][col].value != 0)
		return false;

	if (board[row][col].candidates.contains(candidate))
	{
		board[row][col].candidates.remove(candidate);
		return true;
	}

	return false;
}

bool SudokuBoard::setValue(int row, int col, int value, bool bulk_mode)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
	{
		return false;
	}

	if (value < 0 || value > BOARD_SIZE)
	{
		return false;
	}

	if (board[row][col].isInit)
	{
		return false;
	}

	int old_value = board[row][col].value;

	if (old_value == value)
		return true;

	board[row][col].value = value;

	if (!bulk_mode)
	{
		saveState(row, col, old_value, value);
		redoStack.clear();
	}

	return true;
}

void SudokuBoard::clearValue(int row, int col)
{
	setValue(row, col, 0, false);
}

bool SudokuBoard::isInit(int row, int col) const
{
	if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
	{
		return board[row][col].isInit;
	}
	return false;
}

void SudokuBoard::setInit(int row, int col)
{
	if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
	{
		board[row][col].isInit = true;
	}
}

void SudokuBoard::calculateCandidates(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
	{
		return;
	}

	board[row][col].candidates.clear();

	if (board[row][col].value != 0)
	{
		return;
	}

	for (int value = 1; value <= 9; value++)
	{
		if (isValidPlacement(row, col, value))
		{
			board[row][col].candidates.insert(value);
		}
	}
}

void SudokuBoard::update_all_candidates()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			calculateCandidates(row, col);
		}
	}
}

void SudokuBoard::clear()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			if (!board[row][col].isInit)
			{
				board[row][col].value = 0;
				board[row][col].candidates.clear();
			}
		}
	}

	//    update_all_candidates();
}

bool SudokuBoard::isValidPlacement(int row, int col, int value) const
{
	if (value < 1 || value > BOARD_SIZE)
		return false;

	for (int c = 0; c < BOARD_SIZE; ++c)
	{
		if (c != col && board[row][c].value == value)
			return false;
	}

	for (int r = 0; r < BOARD_SIZE; ++r)
	{
		if (r != row && board[r][col].value == value)
			return false;
	}

	int startRow = (row / BLOCK_SIZE) * BLOCK_SIZE;
	int startCol = (col / BLOCK_SIZE) * BLOCK_SIZE;

	for (int r = 0; r < BLOCK_SIZE; ++r)
	{
		for (int c = 0; c < BLOCK_SIZE; ++c)
		{
			int currRow = startRow + r;
			int currCol = startCol + c;
			if ((currRow != row || currCol != col) && board[currRow][currCol].value == value)
			{
				return false;
			}
		}
	}

	return true;
}

bool SudokuBoard::undo()
{
	if (!canUndo())
	{
		return false;
	}

	Move move = undoStack.pop();

	redoStack.push(move);

	board[move.row][move.col].value = move.old_value;

	return true;
}

bool SudokuBoard::redo()
{
	if (!canRedo())
	{
		return false;
	}

	Move move = redoStack.pop();

	saveState(move.row, move.col, move.old_value, move.new_value);
	board[move.row][move.col].value = move.new_value;

	return true;
}

bool SudokuBoard::canUndo() const
{
	return !undoStack.isEmpty();
}

bool SudokuBoard::canRedo() const
{
	return !redoStack.isEmpty();
}

void SudokuBoard::clearHistory()
{
	undoStack.clear();
	redoStack.clear();
}

void SudokuBoard::saveState(int row, int col, int old_value, int new_value)
{
	Move move = { row, col, old_value, new_value };
	undoStack.push(move);
}

void SudokuBoard::pushUndo(const Move &move)
{
	undoStack.push(move);
}

void SudokuBoard::pushRedo(const Move &move)
{
	redoStack.push(move);
}

SudokuStack SudokuBoard::getRedoStack() const
{
	return redoStack;
}

SudokuStack SudokuBoard::getUndoStack() const
{
	return undoStack;
}

void SudokuBoard::toggleCandidate(int row, int col, int candidate)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return;

	if (board[row][col].value != 0)
		return;

	if (board[row][col].candidates.contains(candidate))
		board[row][col].candidates.remove(candidate);
	else
		board[row][col].candidates.insert(candidate);
}
