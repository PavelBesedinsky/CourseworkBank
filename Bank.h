#pragma once

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "Account.h"
class Bank
{
public:
	Bank(int clientN, int bankID, int terminalID);
	~Bank();
	int getBankID();
	int getCustomerN();
	int getTerminal();
	bool IsCustomer(int customerID);
	int Query(int request, int customerID, int value);
private:
	int _bankID; // Номер банка
	int _customerN; // Общее число клиентов
	int _terminal;
	//double** _customerAccount; // Массив с счетами клиентов (ID клиента; Сумма)
	Account* customerAccounts;
	void CreateCustomerAccounts();
	int FindCustomer(int customerID);
	void Print(int request);
};

