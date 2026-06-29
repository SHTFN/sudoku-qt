#include "sudokugamemanager.h"

void SudokuGameManager::newGame()
{
	timer.reset();
	board.clear();
	board.clearHistory();

	gameStarted = false;
	gameFinished = false;
}

void SudokuGameManager::startGame()
{
	if (!gameStarted)
	{
		timer.start();
		gameStarted = true;
	}
}

bool SudokuGameManager::setValue(int row, int col, int value)
{
	if (board.setValue(row, col, value, false))
	{
		startGame();
		return true;
	}

	return false;
}

bool SudokuGameManager::clearValue(int row, int col)
{
	if (board.isInit(row, col))
		return false;

	if (board.getValue(row, col) == 0)
		return false;

	board.clearValue(row, col);
	startGame();
	return true;
}

bool SudokuGameManager::undo()
{
	return board.undo();
}

bool SudokuGameManager::redo()
{
	return board.redo();
}

Hint SudokuGameManager::makeSolverStep()
{
	Hint hint = SudokuSolver::tryToSolve(board);

	for (const auto &action : hint.setValues)
	{
		board.setValue(action.row, action.col, action.value, false);
	}

	for (const auto &action : hint.removeCandidates)
	{
		board.removeCandidate(action.row, action.col, action.value);
	}

	if (!hint.setValues.isEmpty() || !hint.removeCandidates.isEmpty())
		startGame();

	return hint;
}

bool SudokuGameManager::isSolved() const
{
	return SudokuValidator::isSolved(&board);
}

bool SudokuGameManager::finishGame()
{
	if (!gameFinished && isSolved())
	{
		timer.stop();
		gameFinished = true;
		return true;
	}

	return false;
}
