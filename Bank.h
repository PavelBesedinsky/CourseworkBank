#pragma once

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Account.h"
#include "QueryStruct.h"

class Bank
{
public:
	Bank(int clientN, int bankID, int terminalID);
	~Bank();
	int getBankID();
	int getCustomerN();
	int getTerminal();
	bool IsCustomer(int customerID);
	SQuery Query(SQuery query);
private:
	int _bankID; // ����� �����
	int _customerN; // ����� ����� ��������
	int _terminal;
	//double** _customerAccount; // ������ � ������� �������� (ID �������; �����)
	Account* customerAccounts;
	void CreateCustomerAccounts();
	int FindCustomer(int customerID);
	void Print(int request);
};

