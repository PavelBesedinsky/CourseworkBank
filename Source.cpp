#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include <Windows.h>
#include "Bank.h"
#include "QueryStruct.h"
#include <iostream>
#include "Source.h"
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
	int msgtag1 = 351;

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
	
	const int sizeArr = 5;
	MPI_Datatype queryType;
	MPI_Datatype typeArr[sizeArr] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_CHAR };
	int bsizeArr[sizeArr] = { 1, 1, 1, 1, 100 };
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
	

	if (rank == 0)								// Процесс Сервер
	{
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d server.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
	}

	if (rank >= _rankB && rank < _rankT)		// Процесс Банк
	{
		//
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d bank.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
		//

		_myTerminal = rank + _bankN;			// Определение номера терминала, с которым работает банк
		bank = new Bank(3, rank, _myTerminal);
	}

	if (rank >= _rankT)							// Процесс Терминал
	{
		//
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d terminal.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
		//

		_myBank = rank - _bankN;				// Определение номера банка, с которым работает терминал
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	int count = 0;
	_flag = true;
	int countExit = 0;
	while (_flag)
	{
		if (rank == 0)							// Процесс Сервер
		{
			FILE *f1;
			char str[256];
			sprintf_s(str, "0 server.txt");
			queryStruct._qRequest = 0;
			queryStruct._qResult = 0;
			queryStruct._qSum = 0;
			queryStruct._qText[0] = '0';
			int bankGoal = 0;
			// Получить запрос от банка
			fopen_s(&f1, str, "a+");
			fprintf_s(f1, "\n");
			fprintf_s(f1, "Жду\n");
			fclose(f1);

			// Получить запрос от банка
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			
			if (queryStruct._qResult == -2)
			{
				countExit++;

				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "Принял %d из %d выключений\n", countExit, _bankN);
				fclose(f1);
				//
			
				if (countExit == _bankN)
				{
					//
					fopen_s(&f1, str, "a+");
					fprintf_s(f1, "Выход\n");
					fclose(f1);
					//

					_flag = false; // Убрать флаг?
					break;
				}
			}
			else
			{
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "Получил запрос от банка(%d) клиент(%d)\n", status.MPI_SOURCE, queryStruct._qClientID);
				fclose(f1);
				//

				// Запомнить номер банка, от которого пришел запрос
				int source = status.MPI_SOURCE;
				// Цикл по всем банкам
				for (int i = 1; i <= _bankN; i++)
				{
					fopen_s(&f1, str, "a+");
					fprintf_s(f1, "i = %d && source = %d bank = %d\n", i, source, _bankN);
					fclose(f1);
					// Если банк не из запроса
					if (i != source)
					{
						// Отправить банку запрос
						MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
						
						//
						fopen_s(&f1, str, "a+");
						fprintf_s(f1, "Отправил запрос банку %d\n", i);
						fclose(f1);
						//
					}
				}
				// Получить отбработанный запрос от банка
				MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag1, MPI_COMM_WORLD, &status);
				
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "Получил обработанный запрос клиента(%d) от банка(%d) \n", queryStruct._qClientID, status.MPI_SOURCE);
				fclose(f1);
				//

				// Отправить обработанный запрос банку source
				MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
				
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "Отправил обработанный запрос клиента(%d) в банк(%d)\n", queryStruct._qClientID, source);
				fclose(f1);
				//
			}
		}

		if (rank >= _rankB && rank < _rankT) // Процесс Банк
		{
			FILE *f2;
			char str[256];
			sprintf(str, "%d bank.txt", rank);
		

			// Получить запрос от ANY_SOURCE
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);


			if (queryStruct._qResult == -2)
			{
				countExit++;
				//
				fopen_s(&f2, str, "a+");
				fprintf_s(f2, "Отправил 0 выключение\n");
				fclose(f2);
				//
				
				if (countExit == _bankN)
				{
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);

					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "Выход\n");
					fclose(f2);
					//

					_flag = false; // Убрать флаг?
					break;
				}
			}
			
			// Запрос обработан?
			if (queryStruct._qResult == 1)
			{
				//
				fopen_s(&f2, str, "a+");
				fprintf_s(f2, "Принят обработанный запрос. Отправляю ответ на терминал\n");
				fclose(f2);
				//

				// Отправить ответ на терминал
				MPI_Send(&queryStruct, 1, queryType, _myTerminal, msgtag, MPI_COMM_WORLD);
			}
			else
			{
				int source = status.MPI_SOURCE;

				// Запрос от сервера?
				if (source == bank->getTerminal()) // Если запрос отправлен с терминала
				{
					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "Принят запрос от клиента(%d) с терминала(%d)\n", queryStruct._qClientID, _myTerminal);
					fclose(f2);
					//	

					// Клиент из банка?
					if (bank->IsCustomer(queryStruct._qClientID)) // Если клиент в базе данных банка есть
					{
						
						// Обработать запрос
						queryStruct = bank->Query(queryStruct);	

						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "Клиент(%d) (%d) в базе данных есть, обрабатываю запрос от терминала(%d)\n", queryStruct._qClientID, queryStruct._qResult, _myTerminal);
						fclose(f2);
						//
						
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "Запрос клиента(%d) отправляется на терминал(%d) \n", queryStruct._qClientID, _myTerminal);
						fclose(f2);
						//
						// Отправить ответ терминалу
						MPI_Send(&queryStruct, 1, queryType, _myTerminal, msgtag, MPI_COMM_WORLD);
					}
					else // Если клиента в базе данных есть
					{
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "Клиент(%d) в базе данных не найден, запрос от терминала(%d) отправляется на сервер\n", queryStruct._qClientID, _myTerminal);
						fclose(f2);
						//

						// Отправить необработанный запрос на сервер
						MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);		
					}	
				}
				else if (source == 0)// Если запрос отправлен от сервера
				{

					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "Получен запрос с сервера от клиента(%d)\n", queryStruct._qClientID);
					fclose(f2);
					//

					// Клиент из банка?
					if (bank->IsCustomer(queryStruct._qClientID)) // Если клиент в базе данных есть
					{
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "Клиент(%d) в базе данных есть, обрабатываю запрос с сервера\n", queryStruct._qClientID);
						fclose(f2);
						//

						// Обработать запрос
						queryStruct = bank->Query(queryStruct);

						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "Запрос клиента(%d) обработан и отправляется на сервер\n", queryStruct._qClientID);
						fclose(f2);
						//

						// Отправить обработанный запрос на сервер
						MPI_Send(&queryStruct, 1, queryType, 0, msgtag1, MPI_COMM_WORLD);
					}
				}
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
			queryStruct._qClientID = _MyID;
			queryStruct._qRequest = _MyQuery;
			queryStruct._qSum = _MySum;
			queryStruct._qResult = -1; // Запрос не существует

			if (rank == _rankT)
			{
				_MyID = 1000;
				_MySum = 0;
				if (count == 0)
				{	
					_MyID = 1000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 1)
				{
					_MyID = 2001;
					_MyQuery = 1;
					_MySum = 1000;
					queryStruct._qResult = 0;
				}
				if (count == 2)
				{
					_MyID = 2001;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 3)
				{
					_MyID = 1000;
					_MyQuery = 2;
					_MySum = 5000;
					queryStruct._qResult = 0;
				}
				if (count == 4)
				{
					_MyID = 1000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
			}
			if(rank == _rankT + 1)
			{
				_MyID = 2000;
				if (count == 0)
				{
					_MyID = 1000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 1)
				{
					_MyID = 2000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 2)
				{
					_MyID = 2000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 3)
				{
					_MyID = 2000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 4)
				{
					_MyID = 2000;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
			}
			if (rank == _rankT + 2)
			{
				_MyID = 1001;
				if (count == 0)
				{
					_MyID = 1002;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 1)
				{
					_MyID = 1002;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 2)
				{
					_MyID = 1002;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 3)
				{
					_MyID = 1002;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
				if (count == 4)
				{
					_MyID = 1002;
					_MyQuery = 3;
					queryStruct._qResult = 0;
				}
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

			// Печать результатов
			if (queryStruct._qResult == 0)
			{
				//
				FILE *f3;
				char str[256];
				sprintf(str, "%d terminal.txt", rank);
				fopen_s(&f3, str, "a+");
				//

				if(_MyQuery == 1)
					fprintf_s(f3, "Терминал(%d) отправляет в банк(%d) запрос от клиента(%d): пополнить счёт\n", rank, _myBank, queryStruct._qClientID);
				if (_MyQuery == 2)
					fprintf_s(f3, "Терминал(%d) отправляет в банк(%d) запрос от клиента(%d): обналичивание счёта\n", rank, _myBank, queryStruct._qClientID);
				if (_MyQuery == 3)
					fprintf_s(f3, "Терминал(%d) отправляет в банк(%d) запрос от клиента(%d): запрос баланса\n", rank, _myBank, queryStruct._qClientID);
				fprintf_s(f3, "\n");
				fclose(f3);
				//

				// Отправить запрос банку
				MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);

				// Получить ответ за запрос
				MPI_Recv(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD, &status);
					
				//
				fopen_s(&f3, str, "a+");
				if (_MyQuery == 1)
				{
					fprintf_s(f3, "Терминал(%d) получает ответ от банка(%d) на запрос от клиента(%d): пополнить счёт\n", rank, _myBank, queryStruct._qClientID);
				}
				if (_MyQuery == 2)
				{
					fprintf_s(f3, "Терминал(%d) получает ответ от банка(%d) на запрос от клиента(%d): обналичивание счёта\n", rank, _myBank, queryStruct._qClientID);
				}
				if (_MyQuery == 3)
				{
					fprintf_s(f3, "Терминал(%d) получает ответ от банка(%d) на запрос от клиента(%d): запрос баланса\n", rank, _myBank, queryStruct._qClientID);
				}
				fprintf_s(f3, "Терминал(%d): %s\n", rank, queryStruct._qText);
				fprintf_s(f3, "\n");
				fclose(f3);
				//

				
				//printf("Terminal(%d) %s\n", rank, queryStruct._qText);
			}
			count++;
			if (count > 4 && rank >= _rankT)
			{
				// проблема
				queryStruct._qResult = -2;
				for(int i = 1; i <= _bankN; i++)
					MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
				_flag == false;
				break;
			}
		}
		
		/*if (_flag == false)
			break;*/
	} // Конец while

	FILE *f3;
	char str[256];
	sprintf(str, "%d exit.txt", rank);
	fopen_s(&f3, str, "a+");
	fprintf_s(f3, "Вышел %d\n", rank);
	fclose(f3);

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