#include "sudokufilemanager.h"

#include "sudokuerrors.h"
#include "sudokutimer.h"
#include "sudokuvalidator.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

static QJsonArray stackToJson(const SudokuStack &stack)
{
	QJsonArray array;
	const QVector< Move > moves = stack.to_vector();
	for (const Move &move : moves)
	{
		QJsonObject jsonMove;
		jsonMove["row"] = move.row;
		jsonMove["col"] = move.col;
		jsonMove["old_value"] = move.old_value;
		jsonMove["new_value"] = move.new_value;

		array.append(jsonMove);
	}

	return array;
}

static bool loadStackFromJson(const QJsonArray &array, SudokuBoard &board, bool undo)
{
	for (const QJsonValue &value : array)
	{
		if (!value.isObject())
		{
			return false;
		}

		QJsonObject object = value.toObject();
		Move move;
		move.row = object["row"].toInt(-1);
		move.col = object["col"].toInt(-1);
		move.old_value = object["old_value"].toInt(-1);
		move.new_value = object["new_value"].toInt(-1);

		if (move.col < 0 || move.col >= SudokuBoard::BOARD_SIZE || move.row < 0 || move.row >= SudokuBoard::BOARD_SIZE ||
			move.old_value < 0 || move.old_value > SudokuBoard::BOARD_SIZE || move.new_value < 0 || move.new_value > SudokuBoard::BOARD_SIZE)
		{
			return false;
		}

		if (undo)
		{
			board.pushUndo(move);
		}
		else
		{
			board.pushRedo(move);
		}
	}

	return true;
}

int SudokuFileManager::save(const QString &filename, const SudokuBoard &board, const SudokuTimer &timer)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		qWarning() << "Error with opening out file `" << filename << "`. " << file.errorString();
		return SUDOKU_ERROR_OPEN_FILE;
	}

	QJsonObject root;

	QJsonArray boardArray;
	QJsonArray initArray;
	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		QJsonArray rowArray;
		QJsonArray initRowArray;
		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			rowArray.append(board.getValue(row, col));
			initRowArray.append(board.isInit(row, col));
		}
		boardArray.append(rowArray);
		initArray.append(initRowArray);
	}

	root["board"] = boardArray;
	root["init"] = initArray;
	root["undo"] = stackToJson(board.getUndoStack());
	root["redo"] = stackToJson(board.getRedoStack());
	root["elapsedMs"] = static_cast< double >(timer.elapsedMs());

	QJsonDocument document(root);
	file.write(document.toJson(QJsonDocument::Indented));
	file.close();

	return SUDOKU_SUCCESS;
}

int SudokuFileManager::load(QString &filename, SudokuBoard &board, SudokuTimer &timer)
{
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Error with opening load file `" << filename << "`. " << file.errorString();
		return SUDOKU_ERROR_OPEN_FILE;
	}

	QByteArray data = file.readAll();
	file.close();

	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject())
	{
		return SUDOKU_ERROR_INVALID_DATA;
	}

	QJsonObject root = document.object();

	if (!root["board"].isArray())
	{
		return SUDOKU_ERROR_INVALID_DATA;
	}

	QJsonArray boardArray = root["board"].toArray();

	if (boardArray.size() != SudokuBoard::BOARD_SIZE)
	{
		return SUDOKU_ERROR_INVALID_DATA;
	}

	SudokuBoard tempBoard;

	for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
	{
		if (!boardArray[row].isArray())
		{
			return SUDOKU_ERROR_INVALID_DATA;
		}

		QJsonArray rowArray = boardArray[row].toArray();
		if (rowArray.size() != SudokuBoard::BOARD_SIZE)
		{
			return SUDOKU_ERROR_INVALID_DATA;
		}

		for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
		{
			int value = rowArray[col].toInt(-1);
			if (value < 0 || value > SudokuBoard::BOARD_SIZE)
			{
				return SUDOKU_ERROR_INVALID_DATA;
			}

			tempBoard.setValue(row, col, value, true);
		}
	}

	if (!SudokuValidator::validateBasic(&tempBoard))
		return SUDOKU_ERROR_INCORRECT_BOARD;

	// TODO: возможно тут надо поменять условие на отрицание
	if (root["init"].isArray())
	{
		QJsonArray initArray = root["init"].toArray();
		if (initArray.size() != SudokuBoard::BOARD_SIZE)
		{
			return SUDOKU_ERROR_INVALID_DATA;
		}
		for (int row = 0; row < SudokuBoard::BOARD_SIZE; row++)
		{
			if (!initArray[row].isArray())
			{
				return SUDOKU_ERROR_INVALID_DATA;
			}

			QJsonArray rowArray = initArray[row].toArray();
			if (rowArray.size() != SudokuBoard::BOARD_SIZE)
			{
				return SUDOKU_ERROR_INVALID_DATA;
			}

			for (int col = 0; col < SudokuBoard::BOARD_SIZE; col++)
			{
				if (rowArray[col].toBool())
				{
					tempBoard.setInit(row, col);
				}
			}
		}
	}

	if (root["undo"].isArray())
	{
		if (!loadStackFromJson(root["undo"].toArray(), tempBoard, true))
		{
			return SUDOKU_ERROR_INVALID_DATA;
		}
	}

	if (root["redo"].isArray())
	{
		if (!loadStackFromJson(root["redo"].toArray(), tempBoard, false))
		{
			return SUDOKU_ERROR_INVALID_DATA;
		}
	}

	if (!root["elapsedMs"].isDouble())
	{
		return SUDOKU_ERROR_INVALID_DATA;
	}

	qint64 elapsedMs = static_cast< qint64 >(root["elapsedMs"].toDouble());
	timer.setElapsedMs(elapsedMs);
	board = tempBoard;

	return SUDOKU_SUCCESS;
}
