#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include "sudokustack.h"

#include <QSet>
#include <QVector>

typedef struct Cell
{
	int value = 0;
	bool isInit = false;
	QSet< int > candidates;
} Cell;

class SudokuBoard
{
  private:
	QVector< QVector< Cell > > board;
	SudokuStack undoStack;
	SudokuStack redoStack;
	void saveState(int row, int col, int old_value, int new_value);
	void calculateCandidates(int row, int col);

  public:
	SudokuBoard();
	static constexpr int BOARD_SIZE = 9;
	static constexpr int BLOCK_SIZE = 3;
	int getValue(int row, int col) const;
	bool setValue(int row, int col, int value, bool bulk_mode);
	void clearValue(int row, int col);
	bool isInit(int row, int col) const;
	void setInit(int row, int col);

	void update_all_candidates();
	QSet< int > getCandidates(int row, int col) const;
	bool removeCandidate(int row, int col, int candidate);
	void toggleCandidate(int row, int col, int candidate);

	void clear();

	bool isValidPlacement(int row, int col, int value) const;

	bool undo();
	bool redo();
	//    void clearUndo();
	//    void clearRedo();
	bool canUndo() const;
	bool canRedo() const;
	void clearHistory();

	void pushUndo(const Move &move);
	void pushRedo(const Move &move);

	SudokuStack getUndoStack() const;
	SudokuStack getRedoStack() const;
};

#endif	  // SUDOKUBOARD_H
