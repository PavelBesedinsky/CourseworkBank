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
	int ID(int value);		// ������� ��������� �������������� ��������
	int Sum();				// ������� ��������� ����� ��������
	int Sum(int value);		// ������� ��������� ����� ��������
	Account operator+(int value);
	Account operator-(int value);
	Account operator=(int value);
	Account();
	Account(int id, int sum);
	~Account();
};

