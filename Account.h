#pragma once
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
class Account
{
private:
	int _accountID;			// ������������� ��������
	int _accountSum;		// ���� ��������
	char _accountName[256];	// ��� ���������

public:
	int ID();				// ������� ��������� �������������� ��������
	void ID(int value);		// ������� ��������� �������������� ��������
	int Sum();				// ������� ��������� ����� ��������
	void Sum(int value);		// ������� ��������� ����� ��������
	Account();
	Account(int id, int sum);
	~Account();
};

