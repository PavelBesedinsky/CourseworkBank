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

	/*if (!rank)
		printf("_rankB = %d _rankT = %d\n", _rankB, _rankT);*/

	if (rank == 0)								// ������� ������
	{

	}

	if (rank >= _rankB && rank < _rankT)		// ������� ����
	{

		_myTerminal = rank + _bankN;			// ����������� ������ ���������, � ������� �������� ����
		bank = new Bank(1, rank, _myTerminal);
	}

	if (rank >= _rankT)							// ������� ��������
	{
		_myBank = rank - _bankN;				// ����������� ������ �����, � ������� �������� ��������
	}

	int count = 0;
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
				if (bank->IsCustomer(query[1]) == true) // ���� ������ � ���� ������ ����
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