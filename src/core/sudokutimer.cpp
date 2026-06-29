#include "sudokutimer.h"

SudokuTimer::SudokuTimer() : accumulatedMs(0), running(false) {}

void SudokuTimer::start()
{
	if (!running)
	{
		timer.start();
		running = true;
	}
}

void SudokuTimer::stop()
{
	if (running)
	{
		accumulatedMs += timer.elapsed();
		running = false;
	}
}

void SudokuTimer::reset()
{
	accumulatedMs = 0;
	running = false;
	timer.invalidate();
}

qint64 SudokuTimer::elapsedMs() const
{
	if (running)
		return accumulatedMs + timer.elapsed();

	return accumulatedMs;
}

int SudokuTimer::elapsedSeconds() const
{
	return static_cast< int >(elapsedMs() / 1000);
}

void SudokuTimer::setElapsedMs(qint64 ms)
{
	accumulatedMs = ms;
}
