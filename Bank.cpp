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
	/*for (int i = 0; i < _customerN; i++)
		delete[] _customerAccount[i];
	delete[] _customerAccount;*/
	delete[] customerAccounts;
}

void Bank::CreateCustomerAccounts()
{
	// Добавить чтение из файла
	customerAccounts = new Account[_customerN];
	for (int i = 0; i < _customerN; i++)
	{
		customerAccounts[i].ID(100 + _bankID + i);
		customerAccounts[i].Sum(5000);
		printf("I'm client %d from bank = %d; my id = %d; my sum = %d\n", i, _bankID, customerAccounts[i].ID(), customerAccounts[i].Sum());
	}
	
	printf("Bank: %d has created\n", _bankID);
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