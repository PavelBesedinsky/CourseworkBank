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
	int _bankID;	// Номер банка
					// A number of the bank
	int _customerN; // Общее число клиентов
					// Amount of clients
	int _terminal;
	Account* customerAccounts;
	void CreateCustomerAccounts();
	int FindCustomer(int customerID);
	void Print(int request);
};

