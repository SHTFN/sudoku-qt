#ifndef SUDOKUTIMER_H
#define SUDOKUTIMER_H

#include <QElapsedTimer>

class SudokuTimer
{
  public:
	SudokuTimer();

	void start();
	void stop();
	void reset();

	qint64 elapsedMs() const;
	int elapsedSeconds() const;
	void setElapsedMs(qint64 ms);

  private:
	QElapsedTimer timer;
	qint64 accumulatedMs = 0;
	bool running = false;
};

#endif
