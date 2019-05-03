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
	// Добавить чтение из файла
	customerAccounts = new Account[_customerN];
	for (int i = 0; i < _customerN; i++)
	{
		customerAccounts[i].ID(_bankID * 1000 + i);
		customerAccounts[i].Sum(rand() % (_bankID + 100));
		printf("I'm client %d from bank = %d; my id = %d; my sum = %d\n", i, _bankID, customerAccounts[i].ID(), customerAccounts[i].Sum());
	}
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

int Bank::Query(int request, int customerID, int value)
{
	int answer;
	switch (request)
	{
	case 1: // положить деньги на счет		
		customerAccounts[FindCustomer(customerID)].Sum(value);		// Увеличивает значение суммы владельца счета на value
		answer = customerAccounts[FindCustomer(customerID)].Sum();	// Возвращает текущую сумму на счете пользователя
		Print(request);
		break;
	case 2: // снять деньги с счета
		Print(request);
		break;
	case 3: // посмотреть баланск;
		Print(request);
		break;
	}
	return answer;
}

SQuery Bank::Query(SQuery query)
{
	SQuery answer;
	switch (query._qRequest)
	{
		case 1: // put money in the account	
		{
			customerAccounts[FindCustomer(query._qClientID)].Sum(query._qSum);		// Увеличивает значение суммы владельца счета на value
			
			answer._qResult = 1;
			answer._qSum = customerAccounts[FindCustomer(query._qClientID)].Sum();
			sprintf(query._qText, "Account got money in value = %d", query._qSum);
			Print(query._qRequest);
			break;
		}
		case 2: // give money to the client
		{
			int requestSum = -query._qSum;
			if (query._qSum > customerAccounts[FindCustomer(query._qClientID)].Sum())
			{
				sprintf(answer._qText, "Account haven't enough money in value = %d", query._qSum);
				answer._qResult = 1;
			}
			else
			{
				sprintf(answer._qText, "Take your money in value = %d", query._qSum);
				answer._qResult = 1;
				customerAccounts[FindCustomer(query._qClientID)].Sum(requestSum);
			}
			Print(query._qRequest);
			break;
		}
		case 3: // check the balance of the account
		{
			sprintf(answer._qText, "Your balance is %d", customerAccounts[FindCustomer(query._qClientID)].Sum());
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
	printf("\nRequest %d has been executed\n", request);
}