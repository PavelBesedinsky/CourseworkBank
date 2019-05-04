#include "Bank.h"

Bank::Bank(int clientN, int bankID, int terminalID)
{
	_bankID = bankID;
	_customerN = clientN;
	_terminal = terminalID;
	CreateCustomerAccounts();
	
	
}


Bank::~Bank()
{
	delete[] customerAccounts;
}

void Bank::CreateCustomerAccounts()
{
	// �������� ������ �� �����
	customerAccounts = new Account[_customerN];
	for (int i = 0; i < _customerN; i++)
	{
		customerAccounts[i].ID(_bankID * 1000 + i);
		customerAccounts[i].Sum(rand() % (_bankID + 100));
		printf("I'm client %d from bank = %d; my id = %d; my sum = %d\n", i, _bankID, customerAccounts[i].ID(), customerAccounts[i].Sum());
	}
	printf("\n");
}


int Bank::getBankID()
{
	return _bankID;
}

int Bank::getCustomerN()
{
	return _customerN;
}

int Bank::getTerminal()
{
	return _terminal;
}

bool Bank::IsCustomer(int customerID)
{
	for (int i = 0; i < _customerN; i++)
		if (customerID == customerAccounts[i].ID())
			return true;
	return false;
}

SQuery Bank::Query(SQuery query)
{
	SQuery answer = query;
	switch (query._qRequest)
	{
		case 1: // put money in the account	
		{
			customerAccounts[FindCustomer(query._qClientID)].Sum(query._qSum);		// ����������� �������� ����� ��������� ����� �� value
			
			answer._qResult = 1;
			answer._qSum = customerAccounts[FindCustomer(query._qClientID)].Sum();
			sprintf(answer._qText, "Account[%d] deposit in value = %d", query._qClientID, query._qSum);
			Print(query._qRequest);
			break;
		}
		case 2: // give money to the client
		{
			int requestSum = -query._qSum;
			if (query._qSum > customerAccounts[FindCustomer(query._qClientID)].Sum())
			{
				sprintf(answer._qText, "Account[%d] haven't enough money in value = %d to cash", query._qClientID, query._qSum);
				answer._qResult = 1;
			}
			else
			{
				sprintf(answer._qText, "Account[%d] cashing in value = %d", query._qClientID, query._qSum);
				answer._qResult = 1;
				customerAccounts[FindCustomer(query._qClientID)].Sum(requestSum);
			}
			Print(query._qRequest);
			break;
		}
		case 3: // check the balance of the account
		{
			sprintf(answer._qText, "Account[%d] your balance is %d", query._qClientID, customerAccounts[FindCustomer(query._qClientID)].Sum());
			answer._qResult = 1;
			Print(query._qRequest);
			break;
		}
	}
	return answer;
}


int Bank::FindCustomer(int customerID)
{
	int indx = -1;
	for (int i = 0; i < _customerN; i++)
		if (customerID == customerAccounts[i].ID())
		{
			indx = i;
			break;
		}
	return (indx != -1) ? indx : printf("\nMISTAKE:FindCustomer\n");
}

void Bank::Print(int request)
{
	//printf("\nRequest %d has been executed\n", request);
}