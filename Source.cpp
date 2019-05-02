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
	
	/*if (rank == 0)
	{
		sprintf_s(queryStruct._qText, "Your balance is %d", 23124);
		sprintf_s(queryStruct._qText, "Your balance is %d", 555);
		printf("Text: %s\n", queryStruct._qText);
	}*/

	if (rank == 0)								// ������� ������
	{

	}

	if (rank >= _rankB && rank < _rankT)		// ������� ����
	{

		_myTerminal = rank + _bankN;			// ����������� ������ ���������, � ������� �������� ����
		bank = new Bank(3, rank, _myTerminal);
	}

	if (rank >= _rankT)							// ������� ��������
	{
		_myBank = rank - _bankN;				// ����������� ������ �����, � ������� �������� ��������
	}

	int count = 0;
	_flag = false;
	while (_flag)
	{
		if (rank == 0)							// ������� ������
		{
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			int source = status.MPI_SOURCE;
			for (int i = 1; i < _bankN; i++)
			{
				if (i != source)
				{
					MPI_Send(&queryStruct, 1, queryType, i, msgtag, MPI_COMM_WORLD);
				}
			}
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
		}

		if (rank >= _rankB && rank < _rankT) // ������� ����
		{
			MPI_Recv(&queryStruct, 1, queryType, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			int source = status.MPI_SOURCE;
			if (source == bank->getTerminal()) // ���� ������ ��������� �� ���������
			{
				if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����� ���� ����
				{
					// ���������� ������
					queryStruct = bank->Query(queryStruct);
				}
				else // ���� ������� � ���� ������ ����
				{
					// ��������� �������������� ������ �� ������
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
					// �������� ������������ ������ �� �������
					MPI_Recv(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
				// ��������� ����� ���������
				MPI_Send(&queryStruct, 1, queryType, source, msgtag, MPI_COMM_WORLD);
			}
			else if(source == 0)// ���� ������ ��������� �� �������
			{
				if (bank->IsCustomer(queryStruct._qClientID)) // ���� ������ � ���� ������ ����
				{
					// ���������� ������
					queryStruct = bank->Query(queryStruct);
					// ��������� ������������ ������ �� ������
					MPI_Send(&queryStruct, 1, queryType, 0, msgtag, MPI_COMM_WORLD);
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
			if(_MyQuery != 2)			// ���� ������ �� ������
			{
				int _MySum = rand()%1000 + 50;	// ����� �������
				float fl = rand()%2;
			}
			printf("Terminal(%d)\n", rank);
			// �������:
			// 0 - �������� ������
			// 1 - ����� ������
			// 2 - ���������� ������
			// ������:
			// 3 - ������ ��������
			// 4 - ������ �� ����������
			
			queryStruct._qClientID = _MyID;
			queryStruct._qRequest = _MyQuery;
			queryStruct._qSum = _MySum;
			queryStruct._qResult = -1;

			// ��������� ������ �����
			MPI_Send(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD);
			
			// �������� ����� �� ������
			MPI_Recv(&queryStruct, 1, queryType, _myBank, msgtag, MPI_COMM_WORLD, &status);

			// ������ �����������
			printf("%s\n", queryStruct._qText);
		}
		count++;
		if (count > 1)
			_flag = false;
		if (_flag == false)
			break;
		MPI_Barrier(MPI_COMM_WORLD);
	} // ����� while

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