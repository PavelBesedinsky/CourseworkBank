#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include "Bank.h"
#include <iostream>
// new dev branch

struct Query
{
	int _qID;			// ��� �������
	int _qSum;			// ����� �������
	int _qResult;		// ��������� �������
	char _qText[20];	// ����� ������
};
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

	int query[3] = {0};		// ������: [������][ID �������][�����]

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

		}

		// �������:
		// 0 - �������� ������
		// 1 - ����� ������
		// 2 - ���������� ������
		// ������:
		// 
		//
		//

		if (rank >= _rankB && rank < _rankT) // ������� ����
		{
			int answer = 0;
			MPI_Recv(query, 3, MPI_INT, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			if (status.MPI_SOURCE == bank->getTerminal()) // ���� ������ ��������� �� ���������
			{
				if (bank->IsCustomer(query[1])) // ���� ������ � ���� ������ ����� ���� ����
				{
					// �������� �� ������ �������
					answer = bank->Query(query[0], query[1], query[3]);
					//MPI_Send(&answer)
				}
				else // ���� ������� � ���� ������ ����
				{
					// ��������� ������ �� ������
				}
			}
			else if(status.MPI_SOURCE == 0)// ���� ������ ��������� �� �������
			{
				if (bank->IsCustomer(query[1]) == true) // // ���� ������ � ���� ������ ����
				{

				}
				else // ���� ������� � ���� ������ ����
				{
					// ��������� ����� �� ������
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
			// �������:
			// 0 - �������� ������
			// 1 - ����� ������
			// 2 - ���������� ������
			// ������:
			// 3 - ������ ��������
			// 4 - ������ �� ����������
			
			query[0] = _MyQuery;
			query[1] = _MyID;
			query[2] = _MySum;

			// ��������� ������ �����
			MPI_Send(query, 3, MPI_INT, _myBank, msgtag, MPI_COMM_WORLD);

			// �������� ����� �� ������
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
	MPI_Finalize();
	return 0;
}