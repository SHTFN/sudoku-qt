#include "sudokuvalidator.h"

bool SudokuValidator::checkRows(const SudokuBoard *board)
{
	for (int i = 0; i < SudokuBoard::BOARD_SIZE; i++)
	{
		if (!checkRow(board, i))
		{
			return false;
		}
	}

	return true;
}

bool SudokuValidator::checkRow(const SudokuBoard *board, int row)
{
	QSet< int > uniqs;
	for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
	{
		int value = board->getValue(row, col);
		if (value != 0)
		{
			if (uniqs.contains(value))
			{
				return false;
			}
			uniqs.insert(value);
		}
	}

	return true;
}

bool SudokuValidator::checkCols(const SudokuBoard *board)
{
	for (int i = 0; i < SudokuBoard::BOARD_SIZE; i++)
	{
		if (!checkCol(board, i))
		{
			return false;
		}
	}

	return true;
}

bool SudokuValidator::checkCol(const SudokuBoard *board, int col)
{
	QSet< int > uniqs;
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		int value = board->getValue(row, col);
		if (value != 0)
		{
			if (uniqs.contains(value))
			{
				return false;
			}
			uniqs.insert(value);
		}
	}

	return true;
}

bool SudokuValidator::checkBlocks(const SudokuBoard *board)
{
	for (int block = 0; block < SudokuBoard::BOARD_SIZE; block++)
	{
		if (!checkBlock(board, block))
		{
			return false;
		}
	}

	return true;
}

bool SudokuValidator::checkBlock(const SudokuBoard *board, int block)
{
	QSet< int > uniqs;
	int startRow = (block / SudokuBoard::BLOCK_SIZE) * SudokuBoard::BLOCK_SIZE;
	int startCol = (block % SudokuBoard::BLOCK_SIZE) * SudokuBoard::BLOCK_SIZE;

	for (int row = startRow; row < startRow + SudokuBoard::BLOCK_SIZE; row++)
	{
		for (int col = startCol; col < startCol + SudokuBoard::BLOCK_SIZE; col++)
		{
			int value = board->getValue(row, col);
			if (value != 0)
			{
				if (uniqs.contains(value))
				{
					return false;
				}
				uniqs.insert(value);
			}
		}
	}

	return true;
}

bool SudokuValidator::validateBasic(const SudokuBoard *board)
{
	return checkRows(board) && checkCols(board) && checkBlocks(board);
}

bool SudokuValidator::isSolved(const SudokuBoard *board)
{
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			if (board->getValue(row, col) == 0)
			{
				return false;
			}
		}
	}

	return validateBasic(board);
}
