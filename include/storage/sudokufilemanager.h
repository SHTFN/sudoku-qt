#ifndef SUDOKUFILEMANAGER_H
#define SUDOKUFILEMANAGER_H

#include "sudokuboard.h"
#include "sudokutimer.h"

#include <QString>
#include <QVector>

class SudokuFileManager
{
  public:
	static int save(const QString &filename, const SudokuBoard &board, const SudokuTimer &timer);
	static int load(QString &filename, SudokuBoard &board, SudokuTimer &timer);
};

#endif	  // SUDOKUFILEMANAGER_H
