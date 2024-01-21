#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>
#include "SecondaryFunction.h"

/*
Создать консольное приложение для имитации многопоточного расчёта.
Количество потоков, длина расчёта должны быть заданы переменными.
В консоль во время работы программы должно построчно для каждого потока выводиться:

• Номер потока по порядку;
• Идентификатор потока;
• Заполняющийся индикатор (наподобие прогресс-бара) визуализирующий процесс «расчёта»;
• По завершении работы каждого потока в соответствующей строке выводить суммарное
  время, затраченное при работке потока.
*/

std::atomic<long long> i = 1;
std::atomic<long long> ii = 2;
std::mutex ioMutex;
	
static void funk(const int dataLen, const int pot);

int main(int argc, char** argv)
{
	printHeader(L"Прогресс бар");

	const int potokNum(7);					// кол-во потоков
	std::array<std::thread, potokNum> thrs;	// сами потоки
	const int dataLen(10000000);			// длина данных для расчетов

	std::wcout << L"Номер потока main: " << std::this_thread::get_id() << "\n";
	std::wcout << std::left << L"Количество аппаратных ядер: "
		<< std::thread::hardware_concurrency() << "\n\n"
		<< L"Количество потоков: " << potokNum << "\n"
		<< L"Длина расчета: " << dataLen << "\n";
	
	COORD conspos = getConsolePos();
	int pot(0);
	auto start = std::chrono::steady_clock::now();
	for (auto& t : thrs) t = std::thread(funk, dataLen, ++pot);
	for (auto& t : thrs) t.join();			// ждет окончания join потоков
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> delta = end - start;

	conspos.Y += potokNum + 2;
	setConsolePos(conspos);
	std::wcout << L"Общее время работы: " << delta.count() << "ms\n"
		<< "i count: " << i << "\n"
		<< "ii count: " << ii << "\n\n";

	return 0;
}

static void funk(const int dataLen, const int pot)
{
	ioMutex.lock();
	std::wcout << L"\nПоток: " << pot << "  id: " << std::this_thread::get_id();
	COORD conspos = getConsolePos();
	conspos.X = 21;
	setConsolePos(conspos);
	std::wcout << "[          ]";
	ioMutex.unlock();

	const int lenDivTen = dataLen / 10;
	const auto start = std::chrono::steady_clock::now();
	for (int q = 0; q < dataLen; ++q)
	{
		++i;
		--i;
		++ii;
		if (!(q % lenDivTen))
		{
			++conspos.X;

			ioMutex.lock();
			setConsolePos(conspos);
			std::wcout << "x";
			ioMutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	const auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> delta = end - start;
	const double tm = delta.count();
	conspos.X += 4;

	std::lock_guard<std::mutex> grd(ioMutex);
	setConsolePos(conspos);
	std::wcout << L"Время потока: " << tm << "ms";
}
