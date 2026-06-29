#include "mainwindow.h"

#include "sudokuboardwidget.h"
#include "sudokuerrors.h"
#include "sudokufilemanager.h"
#include "sudokusolver.h"
#include "sudokutimer.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QStringLiteral>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

void MainWindow::enterCandidate(int row, int col, int value)
{
	if (row < 0 || row >= SudokuBoard::BOARD_SIZE || col < 0 || col >= SudokuBoard::BOARD_SIZE)
	{
		statusBar()->showMessage(tr("Incorrect move"), shortStatusBarMessage);
		return;
	}

	if (gameManager.board.getValue(row, col) != 0)
		return;

	gameManager.board.toggleCandidate(row, col, value);
	boardWidget->update();
}

void MainWindow::checkSolved()
{
	if (gameManager.finishGame())
	{
		statusBar()->showMessage(tr("You've solved Sudoku!"), longStatusBarMessage);
	}
}

void MainWindow::saveGame()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save the game"), "", "JSON files (*.json)");

	if (filename.isEmpty())
	{
		return;
	}

	if (!filename.endsWith(".json"))
	{
		filename += ".json";
	}

	int result = SudokuFileManager::save(filename, gameManager.board, gameManager.timer);

	if (result != SUDOKU_SUCCESS)
	{
		QMessageBox::warning(this, tr("Error"), tr("Failed to save file"));
	}
}

void MainWindow::loadGame()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load the game"), "", "JSON files (*.json)");
	if (filename.isEmpty())
		return;

	SudokuTimer timer;
	SudokuBoard tempBoard;
	int result = SudokuFileManager::load(filename, tempBoard, timer);

	if (result != SUDOKU_SUCCESS)
	{
		if (result == SUDOKU_ERROR_INCORRECT_BOARD)
			QMessageBox::warning(this, tr("Error"), tr("Invalid board"));
		else
			QMessageBox::warning(this, tr("Error"), tr("Failed to open file"));
		return;
	}
	gameManager.board = tempBoard;
	gameManager.timer = timer;
	gameManager.gameStarted = false;
	gameManager.gameFinished = false;

	boardWidget->setBoard(&gameManager.board);
	boardWidget->update();

	statusBar()->showMessage(tr("The game is loaded"), shortStatusBarMessage);
}

void MainWindow::updateTimer()
{
	int seconds = gameManager.timer.elapsedSeconds();

	int hours = seconds / SecondsInHour;
	int minutes = (seconds % SecondsInHour) / SecondsInMinute;
	int restSeconds = seconds % SecondsInMinute;

	timerLabel->setText(
		QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(restSeconds, 2, 10, QChar('0')));
}

void MainWindow::undoGame()
{
	if (gameManager.undo())
		boardWidget->update();
}

void MainWindow::redoGame()
{
	if (gameManager.redo())
		boardWidget->update();
}

void MainWindow::newGame()
{
	gameManager.newGame();
	boardWidget->update();
	updateTimer();
	statusBar()->showMessage(tr("New game"), shortStatusBarMessage);
}

void MainWindow::makeSolverStep()
{
	Hint hint = gameManager.makeSolverStep();

	boardWidget->update();
	switch (hint.strategy)
	{
	case SUDOKU_SOLVER_HIDDEN_SINGLES:
		statusBar()->showMessage(tr("The Hidden Singles strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_NAKED_PAIRS:
		statusBar()->showMessage(tr("The Naked Pairs strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_NO_MOVES:
		statusBar()->showMessage(tr("No available moves found"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_HIDDEN_PAIRS:
		statusBar()->showMessage(tr("The Hidden Pairs strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_POINTING_STARS:
		statusBar()->showMessage(tr("The Pointing Stars strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_BOX_LINE_REDUCTION:
		statusBar()->showMessage(tr("The Box Line Reduction strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_NAKED_TRIPLES:
		statusBar()->showMessage(tr("The Naked Triples strategy was applied"), shortStatusBarMessage);
		break;
	case SUDOKU_SOLVER_HIDDEN_TRIPLES:
		statusBar()->showMessage(tr("The Hidden Triples strategy was applied"), shortStatusBarMessage);
		break;
	}

	checkSolved();
}

void MainWindow::handleValueEntered(int row, int col, int value)
{
	if (!gameManager.board.isValidPlacement(row, col, value))
	{
		statusBar()->showMessage(tr("Invalid move"), shortStatusBarMessage);
		return;
	}
	if (gameManager.setValue(row, col, value))
	{
		boardWidget->update();
		checkSolved();
	}
}

void MainWindow::handleCellCleared(int row, int col)
{
	if (gameManager.clearValue(row, col))
	{
		boardWidget->update();
	}
}

void MainWindow::initUi()
{
	setMinimumSize(minWidth, minHeight);

	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	timerLabel = new QLabel("00:00:00", this);
	timerLabel->setAlignment(Qt::AlignHCenter);

	UITimer = new QTimer(this);
	connect(UITimer, &QTimer::timeout, this, &MainWindow::updateTimer);
	UITimer->start(TimerUpdateIntervalMs);

	boardWidget = new SudokuBoardWidget(this);
	boardWidget->setBoard(&gameManager.board);

	helpText = new QPlainTextEdit(this);
	helpText->setReadOnly(true);
	helpText->setFocusPolicy(Qt::NoFocus);

	languageBox = new QComboBox(this);
	languageBox->addItem("English", "en");
	languageBox->addItem("Русский", "ru");
	languageBox->addItem("Română", "ro");

	QStatusBar *statusBar = this->statusBar();
	QLabel *statusBarText = new QLabel(tr("Sudoku"));
	statusBar->addPermanentWidget(statusBarText);
}

void MainWindow::initButtons()
{
	saveBtn = new QPushButton(this);
	loadBtn = new QPushButton(this);
	toggleCandidatesBtn = new QPushButton(this);
	autoCandidatesBtn = new QPushButton(this);
	prevStepBtn = new QPushButton(this);
	nextStepBtn = new QPushButton(this);
	hintBtn = new QPushButton(this);
	newGameBtn = new QPushButton(this);
	memeBtn = new QPushButton(this);
	exitBtn = new QPushButton(this);
}

void MainWindow::initLayouts()
{
	layout = new QVBoxLayout(centralWidget);

	layout->addWidget(timerLabel);
	QHBoxLayout *gameLayout = new QHBoxLayout;
	layout->addLayout(gameLayout, 1);

	gameLayout->addWidget(boardWidget, 1);

	QVBoxLayout *functionalLayout = new QVBoxLayout;
	gameLayout->addLayout(functionalLayout);

	functionalLayout->addWidget(helpText);
	functionalLayout->addWidget(saveBtn);
	functionalLayout->addWidget(loadBtn);
	functionalLayout->addWidget(toggleCandidatesBtn);
	functionalLayout->addWidget(autoCandidatesBtn);
	functionalLayout->addWidget(prevStepBtn);
	functionalLayout->addWidget(nextStepBtn);
	functionalLayout->addWidget(hintBtn);
	functionalLayout->addWidget(newGameBtn);
	functionalLayout->addWidget(memeBtn);
	functionalLayout->addWidget(exitBtn);

	functionalLayout->addWidget(languageBox);

	functionalLayout->addStretch();
}

void MainWindow::initConnect()
{
	connect(boardWidget,
			&SudokuBoardWidget::statusMessageChanged,
			this,
			[this](const QString &message) { statusBar()->showMessage(message, longStatusBarMessage); });
	connect(boardWidget, &SudokuBoardWidget::valueEntered, this, &MainWindow::handleValueEntered);
	connect(boardWidget, &SudokuBoardWidget::cellCleared, this, &MainWindow::handleCellCleared);
	connect(boardWidget, &SudokuBoardWidget::candidateEntered, this, &MainWindow::enterCandidate);

	connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveGame);
	connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadGame);
	connect(
		toggleCandidatesBtn,
		&QPushButton::clicked,
		this,
		[this]()
		{
			boardWidget->inverseDisplayCandidates();
			boardWidget->update();
		});
	connect(
		autoCandidatesBtn,
		&QPushButton::clicked,
		this,
		[this]()
		{
			gameManager.board.update_all_candidates();
			boardWidget->update();
		});
	connect(prevStepBtn, &QPushButton::clicked, this, &MainWindow::undoGame);
	connect(nextStepBtn, &QPushButton::clicked, this, &MainWindow::redoGame);
	connect(hintBtn, &QPushButton::clicked, this, &MainWindow::makeSolverStep);
	connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::newGame);
	connect(memeBtn,
			&QPushButton::clicked,
			this,
			[]() { QDesktopServices::openUrl(QUrl("https://youtu.be/dQw4w9WgXcQ?si=70klYLoly8HuHGQV")); });
	connect(exitBtn, &QPushButton::clicked, this, QApplication::quit);

	connect(languageBox, QOverload< int >::of(&QComboBox::currentIndexChanged), this, &MainWindow::changeLanguage);
}

void MainWindow::changeLanguage(int index)
{
	QString language = languageBox->itemData(index).toString();

	if (!translator.isEmpty())
	{
		qApp->removeTranslator(&translator);
	}

	if (language == "ru")
	{
		if (translator.load(":/translations/app_ru.qm"))
		{
			qApp->installTranslator(&translator);
		}
		else
		{
			qDebug() << "Failed to load Russian translation";
		}
	}
	else if (language == "ro")
	{
		if (translator.load(":/translations/app_ro.qm"))
		{
			qApp->installTranslator(&translator);
		}
		else
		{
			qDebug() << "Failed to load Romanian translation";
		}
	}
	retranslateUi();
}

void MainWindow::retranslateUi()
{
	setWindowTitle(tr("Sudoku"));

	saveBtn->setText(tr("Save"));
	loadBtn->setText(tr("Load"));
	toggleCandidatesBtn->setText(tr("Candidates"));
	autoCandidatesBtn->setText(tr("Auto candidates"));
	prevStepBtn->setText(tr("Previous step"));
	nextStepBtn->setText(tr("Next step"));
	hintBtn->setText(tr("Hint"));
	newGameBtn->setText(tr("New game"));
	memeBtn->setText(tr("Meme"));
	exitBtn->setText(tr("Exit"));

	helpText->setPlainText(tr(QT_TR_NOOP(
		"Help\n\n"
		"Click a cell - select a cell.\n"
		"1-9 - enter a number.\n"
		"Ctrl + 1-9 - add/remove candidate.\n"
		"Backspace/Delete - clear a cell.\n"
		"Tab / Shift+Tab - move to next/previous cell.\n\n"
		"Candidates - show/hide candidates.\n"
		"Auto candidates - calculate candidates.\n"
		"Hint - make one logical step.\n"
		"Previous/Next step - undo/redo.\n"
		"Save/Load - work with JSON file.")));
}

// я попробовал вынести это в .qss файл, но у меня почему-то только половина кнопок красилась :(
void MainWindow::initStyles()
{
	saveBtn->setStyleSheet("QPushButton { background-color: #A3BE8C; border-radius: 3px; color: #FFFFFF}");
	loadBtn->setStyleSheet("QPushButton { background-color: #5E81AC; border-radius: 3px; color: #FFFFFF }");
	toggleCandidatesBtn->setStyleSheet("QPushButton { background-color: #B48EAD; border-radius: 3px; color: #FFFFFF }");
	autoCandidatesBtn->setStyleSheet("QPushButton { background-color: #D08770; border-radius: 3px; color: #2C3E50}");
	prevStepBtn->setStyleSheet("QPushButton {background-color: #B8A99A; border-radius: 3px; color: #2C3E50}");
	nextStepBtn->setStyleSheet("QPushButton { background-color: #A3BE8C; border-radius: 3px; color: #FFFFFF}");
	hintBtn->setStyleSheet("QPushButton { background-color: #7A8B9E; border-radius: 3px; color: #FFFFFF}");
	newGameBtn->setStyleSheet("QPushButton { background-color: #A3BE8C; border-radius: 3px; color: #2C3E50 }");
	memeBtn->setStyleSheet("QPushButton { background-color: #EBCB8B; border-radius: 3px; color: #2C3E50; }");
	exitBtn->setStyleSheet("QPushButton { background-color: #BF616A; border-radius: 3px; color: #FFFFFF}");

	languageBox->setStyleSheet("QComboBox { background-color: #ECEFF4; color: #2e3440; }");

	timerLabel->setStyleSheet("color: #2C3E50;");
	centralWidget->setStyleSheet("background-color: #F9F6F0;");

	helpText->setStyleSheet("color: #000000");
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	initUi();
	initButtons();
	initConnect();
	initLayouts();
	initStyles();

	retranslateUi();
}

MainWindow::~MainWindow() {}
