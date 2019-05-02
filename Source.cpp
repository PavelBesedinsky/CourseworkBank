#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include "Bank.h"
#include <iostream>
// new dev branch

struct Query
{
	int _qID;			// Вид запроса
	int _qSum;			// Сумма запроса
	int _qResult;		// Результат запроса
	char _qText[20];	// Текст ответы
};
int main(int argc, char **argv)
{
	srand(time(NULL));
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

	MPI_Datatype queryType;
	MPI_Datatype typeArr[4] = { MPI_INT, MPI_INT, MPI_INT, MPI_CHAR };
	int bsizeArr[4] = { 1, 1, 1, 20 };
	MPI_Aint dispArr[4];
	Query  _query;
	_query._qID = 0;
	_query._qResult = 0;
	_query._qSum = 0;
	_query._qText[0] = '0';
	MPI_Address(&_query, dispArr);
	MPI_Address(&_query, dispArr + 1);
	MPI_Address(&_query, dispArr + 2);
	MPI_Address(&_query, dispArr + 3);
	MPI_Aint tmp = dispArr[0];
	for (int i = 0; i < 4; i++)
		dispArr[i] = dispArr[i] - tmp;
	MPI_Type_struct(4, bsizeArr, dispArr, typeArr, &queryType);
	MPI_Type_commit(&queryType);
	if (rank == 0)
	{
		_query._qID = 1000;
		_query._qSum = 2000;
		_query._qText[0] = 'H';
		_query._qText[1] = 'e';
		_query._qText[2] = 'l';
		_query._qText[3] = 'l';
		_query._qText[4] = 'o';
		_query._qText[5] = '0';
		MPI_Send(&_query, 1, queryType, 1, msgtag, MPI_COMM_WORLD);
	}
	if (rank == 1)
	{
		MPI_Recv(&_query, 1, queryType, 0, msgtag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		printf("RANK(%d): ID = %d Sum = %d\n", rank, _query._qID, _query._qSum);
		printf("Word: ");
		for (int i = 0; i < 20; i++)
		{
			char s = _query._qText[i];
			if (s == '0')
				break;
			printf("%c", s);
		}
		printf("\n");
	}
	MPI_Type_free(&queryType);

	if (rank == 0)								// Процесс Сервер
	{

	}

	if (rank >= _rankB && rank < _rankT)		// Процесс Банк
	{

		_myTerminal = rank + _bankN;			// Определение номера терминала, с которым работает банк
		bank = new Bank(3, rank, _myTerminal);
	}

	if (rank >= _rankT)							// Процесс Терминал
	{
		_myBank = rank - _bankN;				// Определение номера банка, с которым работает терминал
	}

	int count = 0;
	_flag = false;
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
				if (bank->IsCustomer(query[1])) // Если клиент в базе данных банка есть есть
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