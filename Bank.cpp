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
	for (int i = 0; i < _customerN; i++)
		delete[] _customerAccount[i];
	delete[] _customerAccount;
}

void Bank::CreateCustomerAccounts()
{
	_customerAccount = new double*[_customerN];

	for (int i = 0; i < _customerN; i++)
		_customerAccount[i] = new double[2];

	for (int i = 0; i < _customerN; i++)
		for (int j = 0; j < 2; j++)
			_customerAccount[i][j] = rand();

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
		if (customerID == _customerAccount[i][0])
			return true;
	return false;
}

int Bank::Query(int request, int customerID, int value)
{
	int answer;
	switch (request)
	{
	case 1: // �������� ������ �� ����
		_customerAccount[FindCustomer(customerID)][1] += value; // ����������� �������� ����� ��������� ����� �� value
		answer = _customerAccount[FindCustomer(customerID)][1]; // ���������� ������� ����� �� ����� ������������
		Print(request);
		break;
	case 2: // ����� ������ � �����
		Print(request);
		break;
	case 3: // ���������� �������;
		Print(request);
		break;
	}
	return answer;
}

int Bank::FindCustomer(int customerID)
{
	int indx = -1;
	for (int i = 0; i < _customerN; i++)
		if (customerID == _customerAccount[i][0])
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