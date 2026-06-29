#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sudokuboardwidget.h"
#include "sudokugamemanager.h"

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTranslator>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

  private:
	constexpr static int minWidth = 700;
	constexpr static int minHeight = 600;
	constexpr static int shortStatusBarMessage = 3000;
	constexpr static int longStatusBarMessage = 6000;
	constexpr static int TimerUpdateIntervalMs = 1000;
	constexpr static int SecondsInMinute = 60;
	constexpr static int SecondsInHour = 3600;

	QLabel *timerLabel;
	QTimer *UITimer;
	SudokuGameManager gameManager;
	SudokuBoardWidget *boardWidget;
	QWidget *centralWidget;
	QPlainTextEdit *helpText;

	QComboBox *languageBox;
	QTranslator translator;

	void initUi();
	void initButtons();
	void initLayouts();
	void initStyles();
	void initConnect();

	QVBoxLayout *layout;

	QPushButton *saveBtn;
	QPushButton *loadBtn;
	QPushButton *toggleCandidatesBtn;
	QPushButton *autoCandidatesBtn;
	QPushButton *prevStepBtn;
	QPushButton *nextStepBtn;
	QPushButton *hintBtn;
	QPushButton *newGameBtn;
	QPushButton *memeBtn;
	QPushButton *exitBtn;

	void enterCandidate(int row, int col, int value);
	void checkSolved();
	void saveGame();
	void loadGame();
	void updateTimer();
	void undoGame();
	void redoGame();
	void newGame();
	void makeSolverStep();
	void handleValueEntered(int row, int col, int value);
	void handleCellCleared(int row, int col);

	void changeLanguage(int index);
	void retranslateUi();
};
#endif	  // MAINWINDOW_H
