#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Sudoku!");
	MainWindow w;
	w.show();
	return a.exec();
}
