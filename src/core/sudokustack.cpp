#include "sudokustack.h"

void SudokuStack::push(const Move &move)
{
	stack.push(move);
}

Move SudokuStack::pop()
{
	if (!stack.isEmpty())
		return stack.pop();

	return Move{ -1, -1, -1, -1 };
}

bool SudokuStack::isEmpty() const
{
	return stack.isEmpty();
}

void SudokuStack::clear()
{
	stack.clear();
}

int SudokuStack::size() const
{
	return stack.size();
}

QVector< Move > SudokuStack::to_vector() const
{
	return QVector< Move >(stack);
}
