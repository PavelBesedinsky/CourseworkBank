#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include "Bank.h"
// new dev branch

int main(int argc, char **argv)
{
	int size, rank;
	MPI_Status status;
	//Шапочка
	{
		if (MPI_Init(&argc, &argv) != MPI_SUCCESS) return 1;
		if (MPI_Comm_size(MPI_COMM_WORLD, &size) != MPI_SUCCESS)
		{
			MPI_Finalize();
			return 2;
		}
		if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS)
		{
			MPI_Finalize();
			return 3;
		}
	}

	// rank = 0 - сеть
	// rank = 1...M - банки
	// rank = M + 1 ... N - терминалы
	Bank* bank = NULL;

	bool _flag = true;
	
	int msgtag = 35;
	
	int _bankN;			// количество банков
	int _terminalN;		// количество терминалов
	int _rankB;			// номер процесса, с которого начинаются банки
	int _rankT;			// номер процесса, с которого начинаются терминалы
	int _myTerminal;	// номер терминала у банка
	int _myBank;		// номер банка у терминала
	_bankN = (size - 1) / 2;
	_terminalN = (size - 1) / 2;

	_rankB = 1;
	_rankT = _rankB + _bankN;

	int query[3] = {0};		// Запрос: [Запрос][ID Клиента][Сумма]

	/*if (!rank)
		printf("_rankB = %d _rankT = %d\n", _rankB, _rankT);*/

	if (rank == 0)								// Процесс Сервер
	{

	}

	if (rank >= _rankB && rank < _rankT)		// Процесс Банк
	{

		_myTerminal = rank + _bankN;			// Определение номера терминала, с которым работает банк
		bank = new Bank(1, rank, _myTerminal);
	}

	if (rank >= _rankT)							// Процесс Терминал
	{
		_myBank = rank - _bankN;				// Определение номера банка, с которым работает терминал
	}

	int count = 0;
	while (_flag)
	{
		if (rank == 0)							// Процесс Сервер
		{

		}

		// Запросы:
		// 0 - Положить деньги
		// 1 - Снять деньги
		// 2 - Посмотреть баланс
		// Ответы:
		// 
		//
		//

		if (rank >= _rankB && rank < _rankT) // Процесс Банк
		{
			int answer = 0;
			MPI_Recv(query, 3, MPI_INT, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			if (status.MPI_SOURCE == bank->getTerminal()) // Если запрос отправлен от терминала
			{
				if (bank->IsCustomer(query[1]) == true) // Если клиент в базе данных есть
				{
					// Ответить на запрос клиента
					answer = bank->Query(query[0], query[1], query[3]);
					//MPI_Send(&answer)
				}
				else // Если клиента в базе данных есть
				{
					// Отправить запрос на сервер
				}
			}
			else if(status.MPI_SOURCE == 0)// Если запрос отправлен от сервера
			{
				if (bank->IsCustomer(query[1]) == true) // // Если клиент в базе данных есть
				{

				}
				else // Если клиента в базе данных есть
				{
					// Отправить ответ на сервер
				}
			}
			else
			{
				printf("\nMISTAKE: MESSAGE FROM PROCESSOR: %d\n", status.MPI_SOURCE);
			}
		}

		if (rank >= _rankT) // процессы терминалы
		{
			srand(time(NULL));
			
			// Создание запроса
			int _MyID = 0;				// ID клиента
			int _MyQuery = rand()%3;	// Создание запроса
			int _MySum = 0;				// Сумма запроса
			if(_MyQuery != 2)			// Если запрос на баланс
			{
				int _MySum = rand()%1000 + 50;	// Сумма запроса
				float fl = rand()%2;
			}
			// Запросы:
			// 0 - Положить деньги
			// 1 - Снять деньги
			// 2 - Посмотреть баланс
			// Ответы:
			// 3 - Запрос выполнен
			// 4 - Запрос не выполненфы
			query[0] = _MyQuery;
			query[1] = _MyID;
			query[2] = _MySum;
			// Отправить запрос банку
			MPI_Send(query, 3, MPI_INT, _myBank, msgtag, MPI_COMM_WORLD);

			// Получить ответ за запрос
			MPI_Recv(query, 3, MPI_INT, _myBank, msgtag, MPI_COMM_WORLD, &status);

			switch(query[0])
			{
				case 3:
					{
						break;
					}
				case 4:
					{
						break;
					}
			}
		}
		count++;
		if (count > 100)
			_flag = false;
		if (_flag == false)
			break;
	} // Конец while

	if (rank == 0) // процесс сервер
	{

	}

	if (rank >= _rankB && rank < _rankT) // процесс банки
	{
		delete bank; // удаление банка
	}

	if (rank >= _rankT) // процесс терминалы
	{

	}
	//printf("I'm proc #: %d\n", rank);
	MPI_Finalize();
	return 0;
}