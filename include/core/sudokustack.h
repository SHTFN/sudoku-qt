#ifndef SUDOKUSTACK_H
#define SUDOKUSTACK_H

#include <QMap>
#include <QPair>
#include <QStack>
#include <QVector>

typedef struct Move
{
	int row, col, old_value, new_value;
} Move;

class SudokuStack
{
  public:
	void push(const Move &move);
	Move pop();
	bool isEmpty() const;
	void clear();
	int size() const;
	QVector< Move > to_vector() const;

  private:
	QStack< Move > stack;
};

#endif	  // SUDOKUSTACK_H
