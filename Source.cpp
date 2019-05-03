#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include "Bank.h"
#include "QueryStruct.h"
#include <iostream>
// new dev branch

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

	//int query[3] = {0};		// Запрос: [Запрос][ID Клиента][Сумма]
	
	const int sizeArr = 5;
	MPI_Datatype queryType;
	MPI_Datatype typeArr[sizeArr] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_CHAR };
	int bsizeArr[sizeArr] = { 1, 1, 1, 1, 64 };
	MPI_Aint dispArr[sizeArr];
	SQuery  queryStruct;

	queryStruct._qRequest = 0;
	queryStruct._qResult = 0;
	queryStruct._qSum = 0;
	queryStruct._qText[0] = '0';
	
	MPI_Address(&queryStruct, dispArr);
	MPI_Address(&queryStruct, dispArr + 1);
	MPI_Address(&queryStruct, dispArr + 2);
	MPI_Address(&queryStruct, dispArr + 3);
	MPI_Address(&queryStruct, dispArr + 4);
	MPI_Aint tmp = dispArr[0];
	for (int i = 0; i < sizeArr; i++)
		dispArr[i] = dispArr[i] - tmp;
	MPI_Type_struct(sizeArr, bsizeArr, dispArr, typeArr, &queryType);
	MPI_Type_commit(&queryType);
	//if (rank == 0)
	//{
	//	queryStruct._qRequest = 1000;
	//	queryStruct._qSum = 2000;
	//	/*queryStruct._qText[0] = 'H';
	//	queryStruct._qText[1] = 'e';
	//	queryStruct._qText[2] = 'l';
	//	queryStruct._qText[3] = 'l';
	//	queryStruct._qText[4] = 'o';
	//	queryStruct._qText[5] = '0';*/
	//	char string[] = { "Hello World!!!" };
	//	strncpy(queryStruct._qText, string, strlen(string));
	//	queryStruct._qText[strlen(string)] = '0';
	//	MPI_Send(&queryStruct, 1, queryType, 1, msgtag, MPI_COMM_WORLD);
	//}
	//if (rank == 1)
	//{
	//	MPI_Recv(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
	//	printf("RANK(%d): ID = %d Sum = %d\n", rank, queryStruct._qRequest, queryStruct._qSum);
	//	printf("Word: ");
	//	for (int i = 0; i < 20; i++)
	//	{
	//		char s = queryStruct._qText[i];
	//		if (s == '0')
	//			break;
	//		printf("%c", s);
	//	}
	//	printf("\n");
	//}
	
	if (rank == 0)								// Процесс Сервер
	{

	}

	if (rank >= _rankB && rank < _rankT)		// Процесс Банк
	{
		printf("Bank(%d) connection success!\n", rank);
		_myTerminal = rank + _bankN;			// Определение номера терминала, с которым работает банк
		bank = new Bank(3, rank, _myTerminal);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank >= _rankT)							// Процесс Терминал
	{
		printf("Terminal(%d) connection success!\n", rank);
		_myBank = rank - _bankN;				// Определение номера банка, с которым работает терминал
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	int count = 0;
	_flag = true;
	while (_flag)
	{
		if (rank == 0)							// Процесс Сервер
		{
			int flag = 0;
			MPI_Iprobe(MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &flag, &status);
			// Есть ли сообщение?
			if (flag)
			{
				int bankGoal = 0;
				// Получить запрос от банка
				MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
				// Запомнить номер банка, от которого пришел запрос
				int source = status.MPI_SOURCE;
				// Цикл по всем банкам
				for (int i = 1; i < _bankN; i++)
				{
					// Если банк не из запроса
					if (i != source)
					{
						// Отправить банку запрос
						MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
						// Получить ответ от банка
						MPI_Recv(&bankGoal, 1, MPI_INT, i, msgtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						// Клиент из банка?
						if (bankGoal)
						{
							// Получить отбработанный запрос от банка
							MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
							// Отправить обработанный запрос банку source
							MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
							break;
						}	
					}
				}
			}
		}

		if (rank >= _rankB && rank < _rankT) // Процесс Банк
		{
			// Получить запрос от ANY_SOURCE
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			
			int source = status.MPI_SOURCE;

			// Запрос от сервера?
			if (source == bank->getTerminal()) // Если запрос отправлен от терминала
			{
				// Клиент из банка?
				if (bank->IsCustomer(queryStruct._qClientID)) // Если клиент в базе данных банка есть есть
				{
					// Обработать запрос
					queryStruct = bank->Query(queryStruct);
				}
				else // Если клиента в базе данных есть
				{
					// Отправить необработанный запрос на сервер
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
					// Получить обработанный запрос от сервера
					MPI_Recv(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
				// Отправить ответ терминалу
				MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
			}
			else if(source == 0)// Если запрос отправлен от сервера
			{
				// Клиент из банка?
				if (bank->IsCustomer(queryStruct._qClientID)) // Если клиент в базе данных есть
				{
					int bankGoal = 1;
					// Сообщить серверу, что клиент из банка
					MPI_Send(&bankGoal, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
					// Обработать запрос
					queryStruct = bank->Query(queryStruct);
					// Отправить обработанный запрос на сервер
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
				}
				else
				{
					int bankGoal = 0;
					// Сообщить серверу, что клиент не из банка
					MPI_Send(&bankGoal, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
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
			sprintf(queryStruct._qText, "empty0");
			if(_MyQuery != 2)			// Если запрос на баланс
			{
				int _MySum = rand()%1000 + 50;	// Сумма запроса
				float fl = rand()%2;
			}
			if (rank == 3)
			{
				_MyID = 2001;
				_MyQuery = 3;
			}
			if(rank == 4)
			{
				_MyID = 1002;
				_MyQuery = 3;
			}
			// Запросы:
			// 1 - Положить деньги
			// 2 - Снять деньги
			// 3 - Посмотреть баланс
			// Ответы:
			// 4 - Запрос выполнен
			// 5 - Запрос не выполненфы
			
			queryStruct._qClientID = _MyID;
			queryStruct._qRequest = _MyQuery;
			queryStruct._qSum = _MySum;
			queryStruct._qResult = -1;

			// Отправить запрос банку
			MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);
			
			// Получить ответ за запрос
			MPI_Recv(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD, &status);

			// Печать результатов
			printf("Terminal(%d) %s\n", rank, queryStruct._qText);
		}
		count++;
		if (count > 1)
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
	MPI_Type_free(&queryType);
	MPI_Finalize();
	return 0;
}