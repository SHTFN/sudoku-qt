#ifndef SUDOKUBOARDWIDGET_H
#define SUDOKUBOARDWIDGET_H

#include "sudokuboard.h"

#include <QString>
#include <QWidget>

class SudokuBoardWidget : public QWidget
{
	Q_OBJECT

  private:
	static constexpr int minWidth = 450;
	static constexpr int minHeight = 450;
	static constexpr int NO_SELECTION = -1;
	static constexpr int MainGridLineWidth = 5;
	static constexpr int InnerGridLineWidth = 1;
	static constexpr int CandidateFontScale = 5;
	int selectedRow = NO_SELECTION;
	int selectedCol = NO_SELECTION;
	bool displayCandidates = true;
	SudokuBoard *board = nullptr;
	void drawNumbers(QPainter &painter, int startX, int startY, int cellSize);
	void drawSelected(QPainter &painter, int startX, int startY, int boardSize);
	void drawGrid(QPainter &painter, int startX, int startY, int cellSize, int boardSize);
	void drawCandidates(QPainter &painter, int startX, int startY, int cellSize, int boardSize);

	void moveForeward();
	void moveBackward();

  public:
	explicit SudokuBoardWidget(QWidget *parent = nullptr);
	void setBoard(SudokuBoard *newBoard);
	void inverseDisplayCandidates();

  protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
  signals:
	void statusMessageChanged(const QString &message);
	void valueEntered(int row, int col, int value);
	void cellCleared(int row, int col);
	void candidateEntered(int row, int col, int value);
};

#endif	  // SUDOKUBOARDWIDGET_H
