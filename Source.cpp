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

	//int query[3] = {0};		// ������: [������][ID �������][�����]
	
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
	
	if (rank == 0)								// ������� ������
	{

	}

	if (rank >= _rankB && rank < _rankT)		// ������� ����
	{
		printf("Bank(%d) connection success!\n", rank);
		_myTerminal = rank + _bankN;			// ����������� ������ ���������, � ������� �������� ����
		bank = new Bank(3, rank, _myTerminal);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank >= _rankT)							// ������� ��������
	{
		printf("Terminal(%d) connection success!\n", rank);
		_myBank = rank - _bankN;				// ����������� ������ �����, � ������� �������� ��������
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	int count = 0;
	_flag = true;
	while (_flag)
	{
		if (rank == 0)							// ������� ������
		{
			int bankGoal = 0;
			// �������� ������ �� �����
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			if (queryStruct._qResult == -2)
			{
				_flag = false;
				break;
			}
			FILE *f1;
			fopen_s(&f1, "file0.txt", "a+");
			fprintf_s(f1, "������� ������ �� %d\n", status.MPI_SOURCE);
			fclose(f1);
			// ��������� ����� �����, �� �������� ������ ������
			int source = status.MPI_SOURCE;
			// ���� �� ���� ������
			for (int i = 1; i < _bankN; i++)
			{
				// ���� ���� �� �� �������
				if (i != source)
				{
					// ��������� ����� ������
					MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
					fopen_s(&f1, "file0.txt", "a+");
					fprintf_s(f1, "�������� ������ ����� %d\n", i);
					fclose(f1);
					// �������� ����� �� �����
					MPI_Recv(&bankGoal, 1, MPI_INT, i, msgtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					fopen_s(&f1, "file0.txt", "a+");
					fprintf_s(f1, "���� ������ %d\n", bankGoal);
					fclose(f1);
					// ������ �� �����?
					if (bankGoal)
					{
						// �������� ������������� ������ �� �����
						MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
						// ��������� ������������ ������ ����� source
						MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
						break;
					}
				}
			}
		}

		if (rank >= _rankB && rank < _rankT) // ������� ����
		{
			FILE *f2;
			char str[256];
			sprintf(str, "file %d.txt", rank);
			/*fopen_s(&f2, str, "a+");
			fprintf_s(f2, "count %d\n", count);
			fclose(f2);*/
		
			// �������� ������ �� ANY_SOURCE
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			fopen_s(&f2, str, "a+");
			fprintf_s(f2, "count %d queryStruct._qResult = %d\n", count, queryStruct._qResult);
			fclose(f2);
			if (queryStruct._qResult == -2)
			{
				
				MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
				_flag = false;
				break;
			}
			int source = status.MPI_SOURCE;

			// ������ �� �������?
			if (source == bank->getTerminal()) // ���� ������ ��������� �� ���������
			{
				// ������ �� �����?
				if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����� ���� ����
				{
					// ���������� ������
					queryStruct = bank->Query(queryStruct);
				}
				else // ���� ������� � ���� ������ ����
				{
					//printf("You have mistaken with a terminal!\n");
					//sprintf(queryStruct._qText, "Account[%d] I'm have mistaken with a terminal!", queryStruct._qClientID);
					// ��������� �������������� ������ �� ������
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
					// �������� ������������ ������ �� �������
					MPI_Recv(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
				// ��������� ����� ���������
				MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
			}
			else if (source == 0)// ���� ������ ��������� �� �������
			{
				// ������ �� �����?
				if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����
				{
					int bankGoal = 1;
					// �������� �������, ��� ������ �� �����
					MPI_Send(&bankGoal, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
					// ���������� ������
					queryStruct = bank->Query(queryStruct);
					// ��������� ������������ ������ �� ������
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
				}
				else
				{
					int bankGoal = 0;
					// �������� �������, ��� ������ �� �� �����
					MPI_Send(&bankGoal, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
				}
			}
			else
			{
				printf("\nMISTAKE: MESSAGE FROM PROCESSOR: %d\n", status.MPI_SOURCE);
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
			if (rank == 3)
			{
				_MyID = 1001;
				_MySum = 0;
				if (count == 0)
				{	
					_MyID = 1000;
					_MyQuery = 3;
				}
				if (count == 1)
				{
					_MyQuery = 1;
					_MySum = 1000;
				}
				if (count == 2)
					_MyQuery = 3;
				if (count == 3)
				{
					_MyQuery = 2;
					_MySum = 5000;
				}
				if (count == 4)
					_MyQuery = 3;
			}
			if(rank == 4)
			{
				_MyID = 2000;
				if (count == 0)
				{
					_MyID = 1000;
					_MyQuery = 3;
				}
				if (count == 1)
				{
					_MyID = 2000;
					_MyQuery = 3;
				}
				if (count == 2)
				{
					_MyID = 2001;
					_MyQuery = 3;
				}
				if (count == 3)
				{
					_MyID = 2002;
					_MyQuery = 3;
				}
				if (count == 4)
				{
					_MyID = 2002;
					_MyQuery = 3;
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
			queryStruct._qResult = -1;

			// ��������� ������ �����
			MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);
			
			// �������� ����� �� ������
			MPI_Recv(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD, &status);

			// ������ �����������
			printf("Terminal(%d) %s\n", rank, queryStruct._qText);
			
			count++;
			if (count > 4 && rank >= _rankT)
			{
				queryStruct._qResult = -2;
				MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);
				_flag == false;
				break;
			}
		}
		
		/*if (_flag == false)
			break;*/
	} // ����� while

	FILE *f3;
	char str[256];
	sprintf(str, "exit %d.txt", rank);
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