#include "sudokuboardwidget.h"

#include "sudokucolors.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

SudokuBoardWidget::SudokuBoardWidget(QWidget *parent) : QWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setMinimumSize(minWidth, minHeight);
}

void SudokuBoardWidget::setBoard(SudokuBoard *newBoard)
{
	board = newBoard;
	update();
}

void SudokuBoardWidget::drawCandidates(QPainter &painter, int startX, int startY, int cellSize, int boardSize)
{
	Q_UNUSED(boardSize);

	painter.setPen(Theme::Colors::Candidates);

	QFont font = painter.font();
	font.setPointSize(cellSize / CandidateFontScale);
	painter.setFont(font);

	int candidateCellSize = cellSize / SudokuBoard::BLOCK_SIZE;

	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			if (board->getValue(row, col) != 0)
				continue;

			QSet< int > candidates = board->getCandidates(row, col);

			int cellX = startX + col * cellSize;
			int cellY = startY + row * cellSize;

			for (const int &candidate : candidates)
			{
				int index = candidate - 1;

				int candidateRow = index / SudokuBoard::BLOCK_SIZE;
				int candidateCol = index % SudokuBoard::BLOCK_SIZE;

				QRect candidateRect(cellX + candidateCol * candidateCellSize, cellY + candidateRow * candidateCellSize, candidateCellSize, candidateCellSize);

				painter.drawText(candidateRect, Qt::AlignCenter, QString::number(candidate));
			}
		}
	}
}

void SudokuBoardWidget::drawNumbers(QPainter &painter, int startX, int startY, int cellSize)
{
	QFont font = painter.font();
	font.setPointSize(cellSize / 2);
	font.setBold(true);
	painter.setFont(font);

	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			int value = board->getValue(row, col);

			if (value == 0)
			{
				continue;
			}

			QRect cellRect(startX + col * cellSize, startY + row * cellSize, cellSize, cellSize);

			if (board->isInit(row, col))
			{
				painter.setPen(Theme::Colors::InitNum);
			}
			else
			{
				painter.setPen(Theme::Colors::UserNum);
			}
			painter.drawText(cellRect, Qt::AlignCenter, QString::number(value));
		}
	}
}

void SudokuBoardWidget::drawGrid(QPainter &painter, int startX, int startY, int cellSize, int boardSize)
{
	for (int i = 0; i <= SudokuBoard::BOARD_SIZE; i++)
	{
		int lineWidth = (i % SudokuBoard::BLOCK_SIZE == 0) ? MainGridLineWidth : InnerGridLineWidth;
		painter.setPen(QPen(Theme::Colors::GridLineMain, lineWidth));

		int x = startX + i * cellSize;
		int y = startY + i * cellSize;
		painter.drawLine(x, startY, x, startY + boardSize);
		painter.drawLine(startX, y, startX + boardSize, y);
	}
}

void SudokuBoardWidget::drawSelected(QPainter &painter, int startX, int startY, int cellSize)
{
	if (selectedCol == NO_SELECTION || selectedRow == NO_SELECTION)
		return;

	QRect selectedRect(startX + selectedCol * cellSize, startY + selectedRow * cellSize, cellSize, cellSize);

	painter.fillRect(selectedRect, Theme::Colors::SelectoinHighLight);
}

void SudokuBoardWidget::paintEvent(QPaintEvent *event)
{
	if (!board)
		return;

	Q_UNUSED(event);

	QPainter painter(this);

	int size = qMin(width(), height());
	int cellSize = size / SudokuBoard::BOARD_SIZE;
	int boardSize = cellSize * SudokuBoard::BOARD_SIZE;
	int startX = (width() - boardSize) / 2;
	int startY = (height() - boardSize) / 2;

	drawSelected(painter, startX, startY, cellSize);
	drawNumbers(painter, startX, startY, cellSize);
	if (displayCandidates)
		drawCandidates(painter, startX, startY, cellSize, boardSize);
	drawGrid(painter, startX, startY, cellSize, boardSize);
}

void SudokuBoardWidget::mousePressEvent(QMouseEvent *event)
{
	int size = qMin(width(), height());
	int cellSize = size / SudokuBoard::BOARD_SIZE;
	int boardSize = cellSize * SudokuBoard::BOARD_SIZE;

	int startX = (width() - boardSize) / 2;
	int startY = (height() - boardSize) / 2;

	int x = event->pos().x() - startX;
	int y = event->pos().y() - startY;

	if (x < 0 || y < 0 || x >= boardSize || y >= boardSize)
	{
		selectedRow = NO_SELECTION;
		selectedCol = NO_SELECTION;
		update();
		return;
	}

	selectedRow = y / cellSize;
	selectedCol = x / cellSize;

	setFocus();
	update();
}

void SudokuBoardWidget::keyPressEvent(QKeyEvent *event)
{
	if (selectedRow == NO_SELECTION || selectedCol == NO_SELECTION)
	{
		return;
	}

	int key = event->key();
	bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

	if (key >= Qt::Key_1 && key <= Qt::Key_9)
	{
		int value = key - Qt::Key_0;

		if (ctrlPressed)
			emit candidateEntered(selectedRow, selectedCol, value);
		else
			emit valueEntered(selectedRow, selectedCol, value);
		return;
	}

	if (key == Qt::Key_Backspace || key == Qt::Key_Delete)
	{
		emit cellCleared(selectedRow, selectedCol);
		return;
	}

	if (key == Qt::Key_Tab)
	{
		moveForeward();
		update();
		return;
	}

	if (key == Qt::Key_Backtab)
	{
		moveBackward();
		update();
		return;
	}
}

void SudokuBoardWidget::moveForeward()
{
	int blockRow = selectedRow / SudokuBoard::BLOCK_SIZE;
	int blockCol = selectedCol / SudokuBoard::BLOCK_SIZE;

	int localRow = selectedRow % SudokuBoard::BLOCK_SIZE;
	int localCol = selectedCol % SudokuBoard::BLOCK_SIZE;

	int localIndex = localRow * SudokuBoard::BLOCK_SIZE + localCol;

	localIndex++;

	if (localIndex < SudokuBoard::BLOCK_SIZE * SudokuBoard::BLOCK_SIZE)
	{
		localRow = localIndex / SudokuBoard::BLOCK_SIZE;
		localCol = localIndex % SudokuBoard::BLOCK_SIZE;
	}
	else
	{
		blockCol++;

		if (blockCol >= SudokuBoard::BLOCK_SIZE)
		{
			blockCol = 0;
			blockRow++;

			if (blockRow >= SudokuBoard::BLOCK_SIZE)
				blockRow = 0;
		}

		localIndex = 0;

		localRow = 0;
		localCol = 0;
	}

	selectedRow = blockRow * SudokuBoard::BLOCK_SIZE + localRow;
	selectedCol = blockCol * SudokuBoard::BLOCK_SIZE + localCol;
}

void SudokuBoardWidget::moveBackward()
{
	int blockRow = selectedRow / SudokuBoard::BLOCK_SIZE;
	int blockCol = selectedCol / SudokuBoard::BLOCK_SIZE;

	int localRow = selectedRow % SudokuBoard::BLOCK_SIZE;
	int localCol = selectedCol % SudokuBoard::BLOCK_SIZE;

	int localIndex = localRow * SudokuBoard::BLOCK_SIZE + localCol;

	localIndex--;

	if (localIndex >= 0)
	{
		localRow = localIndex / SudokuBoard::BLOCK_SIZE;
		localCol = localIndex % SudokuBoard::BLOCK_SIZE;
	}
	else
	{
		blockCol--;

		if (blockCol < 0)
		{
			blockCol = SudokuBoard::BLOCK_SIZE - 1;
			blockRow--;

			if (blockRow < 0)
				blockRow = SudokuBoard::BLOCK_SIZE - 1;
		}

		localIndex = SudokuBoard::BLOCK_SIZE * SudokuBoard::BLOCK_SIZE - 1;

		localRow = localIndex / SudokuBoard::BLOCK_SIZE;
		localCol = localIndex % SudokuBoard::BLOCK_SIZE;
	}

	selectedRow = blockRow * SudokuBoard::BLOCK_SIZE + localRow;
	selectedCol = blockCol * SudokuBoard::BLOCK_SIZE + localCol;
}

void SudokuBoardWidget::inverseDisplayCandidates()
{
	displayCandidates = !displayCandidates;

	if (displayCandidates)
		emit statusMessageChanged(tr("Display of candidates is enabled"));
	else
		emit statusMessageChanged(tr("Display of candidates is disabled"));

	update();
}
