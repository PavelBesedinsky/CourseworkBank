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
	//�������
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

	// rank = 0 - ����
	// rank = 1...M - �����
	// rank = M + 1 ... N - ���������
	Bank* bank = NULL;

	bool _flag = true;
	
	int msgtag = 35;
	int msgtag1 = 351;

	int _bankN;			// ���������� ������
	int _terminalN;		// ���������� ����������
	int _rankB;			// ����� ��������, � �������� ���������� �����
	int _rankT;			// ����� ��������, � �������� ���������� ���������
	int _myTerminal;	// ����� ��������� � �����
	int _myBank;		// ����� ����� � ���������
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
	

	if (rank == 0)								// ������� ������
	{
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d server.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
	}

	if (rank >= _rankB && rank < _rankT)		// ������� ����
	{
		//
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d bank.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
		//

		_myTerminal = rank + _bankN;			// ����������� ������ ���������, � ������� �������� ����
		bank = new Bank(3, rank, _myTerminal);
	}

	if (rank >= _rankT)							// ������� ��������
	{
		//
		FILE *finit;
		char strinit[256];
		sprintf(strinit, "%d terminal.txt", rank);
		fopen_s(&finit, strinit, "w");
		fclose(finit);
		//

		_myBank = rank - _bankN;				// ����������� ������ �����, � ������� �������� ��������
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	int count = 0;
	_flag = true;
	int countExit = 0;
	while (_flag)
	{
		if (rank == 0)							// ������� ������
		{
			FILE *f1;
			char str[256];
			sprintf_s(str, "0 server.txt");
			queryStruct._qRequest = 0;
			queryStruct._qResult = 0;
			queryStruct._qSum = 0;
			queryStruct._qText[0] = '0';
			int bankGoal = 0;
			// �������� ������ �� �����
			fopen_s(&f1, str, "a+");
			fprintf_s(f1, "\n");
			fprintf_s(f1, "���\n");
			fclose(f1);

			// �������� ������ �� �����
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			
			if (queryStruct._qResult == -2)
			{
				countExit++;

				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "������ %d �� %d ����������\n", countExit, _bankN);
				fclose(f1);
				//
			
				if (countExit == _bankN)
				{
					//
					fopen_s(&f1, str, "a+");
					fprintf_s(f1, "�����\n");
					fclose(f1);
					//

					_flag = false; // ������ ����?
					break;
				}
			}
			else
			{
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "������� ������ �� �����(%d) ������(%d)\n", status.MPI_SOURCE, queryStruct._qClientID);
				fclose(f1);
				//

				// ��������� ����� �����, �� �������� ������ ������
				int source = status.MPI_SOURCE;
				// ���� �� ���� ������
				for (int i = 1; i <= _bankN; i++)
				{
					fopen_s(&f1, str, "a+");
					fprintf_s(f1, "i = %d && source = %d bank = %d\n", i, source, _bankN);
					fclose(f1);
					// ���� ���� �� �� �������
					if (i != source)
					{
						// ��������� ����� ������
						MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
						
						//
						fopen_s(&f1, str, "a+");
						fprintf_s(f1, "�������� ������ ����� %d\n", i);
						fclose(f1);
						//
					}
				}
				// �������� ������������� ������ �� �����
				MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag1, MPI_COMM_WORLD, &status);
				
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "������� ������������ ������ �������(%d) �� �����(%d) \n", queryStruct._qClientID, status.MPI_SOURCE);
				fclose(f1);
				//

				// ��������� ������������ ������ ����� source
				MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
				
				//
				fopen_s(&f1, str, "a+");
				fprintf_s(f1, "�������� ������������ ������ �������(%d) � ����(%d)\n", queryStruct._qClientID, source);
				fclose(f1);
				//
			}
		}

		if (rank >= _rankB && rank < _rankT) // ������� ����
		{
			FILE *f2;
			char str[256];
			sprintf(str, "%d bank.txt", rank);
		

			// �������� ������ �� ANY_SOURCE
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);


			if (queryStruct._qResult == -2)
			{
				countExit++;
				//
				fopen_s(&f2, str, "a+");
				fprintf_s(f2, "�������� 0 ����������\n");
				fclose(f2);
				//
				
				if (countExit == _bankN)
				{
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);

					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "�����\n");
					fclose(f2);
					//

					_flag = false; // ������ ����?
					break;
				}
			}
			
			// ������ ���������?
			if (queryStruct._qResult == 1)
			{
				//
				fopen_s(&f2, str, "a+");
				fprintf_s(f2, "������ ������������ ������. ��������� ����� �� ��������\n");
				fclose(f2);
				//

				// ��������� ����� �� ��������
				MPI_Send(&queryStruct, 1, queryType, _myTerminal, msgtag, MPI_COMM_WORLD);
			}
			else
			{
				int source = status.MPI_SOURCE;

				// ������ �� �������?
				if (source == bank->getTerminal()) // ���� ������ ��������� � ���������
				{
					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "������ ������ �� �������(%d) � ���������(%d)\n", queryStruct._qClientID, _myTerminal);
					fclose(f2);
					//	

					// ������ �� �����?
					if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����� ����
					{
						
						// ���������� ������
						queryStruct = bank->Query(queryStruct);	

						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "������(%d) (%d) � ���� ������ ����, ����������� ������ �� ���������(%d)\n", queryStruct._qClientID, queryStruct._qResult, _myTerminal);
						fclose(f2);
						//
						
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "������ �������(%d) ������������ �� ��������(%d) \n", queryStruct._qClientID, _myTerminal);
						fclose(f2);
						//
						// ��������� ����� ���������
						MPI_Send(&queryStruct, 1, queryType, _myTerminal, msgtag, MPI_COMM_WORLD);
					}
					else // ���� ������� � ���� ������ ����
					{
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "������(%d) � ���� ������ �� ������, ������ �� ���������(%d) ������������ �� ������\n", queryStruct._qClientID, _myTerminal);
						fclose(f2);
						//

						// ��������� �������������� ������ �� ������
						MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);		
					}	
				}
				else if (source == 0)// ���� ������ ��������� �� �������
				{

					//
					fopen_s(&f2, str, "a+");
					fprintf_s(f2, "������� ������ � ������� �� �������(%d)\n", queryStruct._qClientID);
					fclose(f2);
					//

					// ������ �� �����?
					if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����
					{
						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "������(%d) � ���� ������ ����, ����������� ������ � �������\n", queryStruct._qClientID);
						fclose(f2);
						//

						// ���������� ������
						queryStruct = bank->Query(queryStruct);

						//
						fopen_s(&f2, str, "a+");
						fprintf_s(f2, "������ �������(%d) ��������� � ������������ �� ������\n", queryStruct._qClientID);
						fclose(f2);
						//

						// ��������� ������������ ������ �� ������
						MPI_Send(&queryStruct, 1, queryType, 0, msgtag1, MPI_COMM_WORLD);
					}
				}
			}
		}

		if (rank >= _rankT) // �������� ���������
		{
			srand(time(NULL));
			
			// �������� �������
			int _MyID = 0;				// ID �������
			int _MyQuery = rand()%3;	// �������� �������
			int _MySum = 0;				// ����� �������
			sprintf(queryStruct._qText, "empty0");
			if(_MyQuery != 2)			// ���� ������ �� ������
			{
				int _MySum = rand()%1000 + 50;	// ����� �������
				float fl = rand()%2;
			}
			queryStruct._qClientID = _MyID;
			queryStruct._qRequest = _MyQuery;
			queryStruct._qSum = _MySum;
			queryStruct._qResult = -1; // ������ �� ����������

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
			// �������:
			// 1 - �������� ������
			// 2 - ����� ������
			// 3 - ���������� ������
			// ������:
			// 4 - ������ ��������
			// 5 - ������ �� ����������

			queryStruct._qClientID = _MyID;
			queryStruct._qRequest = _MyQuery;
			queryStruct._qSum = _MySum;

			// ������ �����������
			if (queryStruct._qResult == 0)
			{
				//
				FILE *f3;
				char str[256];
				sprintf(str, "%d terminal.txt", rank);
				fopen_s(&f3, str, "a+");
				//

				if(_MyQuery == 1)
					fprintf_s(f3, "��������(%d) ���������� � ����(%d) ������ �� �������(%d): ��������� ����\n", rank, _myBank, queryStruct._qClientID);
				if (_MyQuery == 2)
					fprintf_s(f3, "��������(%d) ���������� � ����(%d) ������ �� �������(%d): ������������� �����\n", rank, _myBank, queryStruct._qClientID);
				if (_MyQuery == 3)
					fprintf_s(f3, "��������(%d) ���������� � ����(%d) ������ �� �������(%d): ������ �������\n", rank, _myBank, queryStruct._qClientID);
				fprintf_s(f3, "\n");
				fclose(f3);
				//

				// ��������� ������ �����
				MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);

				// �������� ����� �� ������
				MPI_Recv(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD, &status);
					
				//
				fopen_s(&f3, str, "a+");
				if (_MyQuery == 1)
				{
					fprintf_s(f3, "��������(%d) �������� ����� �� �����(%d) �� ������ �� �������(%d): ��������� ����\n", rank, _myBank, queryStruct._qClientID);
				}
				if (_MyQuery == 2)
				{
					fprintf_s(f3, "��������(%d) �������� ����� �� �����(%d) �� ������ �� �������(%d): ������������� �����\n", rank, _myBank, queryStruct._qClientID);
				}
				if (_MyQuery == 3)
				{
					fprintf_s(f3, "��������(%d) �������� ����� �� �����(%d) �� ������ �� �������(%d): ������ �������\n", rank, _myBank, queryStruct._qClientID);
				}
				fprintf_s(f3, "��������(%d): %s\n", rank, queryStruct._qText);
				fprintf_s(f3, "\n");
				fclose(f3);
				//

				
				//printf("Terminal(%d) %s\n", rank, queryStruct._qText);
			}
			count++;
			if (count > 4 && rank >= _rankT)
			{
				// ��������
				queryStruct._qResult = -2;
				for(int i = 1; i <= _bankN; i++)
					MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
				_flag == false;
				break;
			}
		}
		
		/*if (_flag == false)
			break;*/
	} // ����� while

	FILE *f3;
	char str[256];
	sprintf(str, "%d exit.txt", rank);
	fopen_s(&f3, str, "a+");
	fprintf_s(f3, "����� %d\n", rank);
	fclose(f3);

	if (rank == 0) // ������� ������
	{

	}

	if (rank >= _rankB && rank < _rankT) // ������� �����
	{
		delete bank; // �������� �����
	}

	if (rank >= _rankT) // ������� ���������
	{

	}
	//printf("I'm proc #: %d\n", rank);
	MPI_Type_free(&queryType);
	MPI_Finalize();
	return 0;
}