#include "Account.h"



int Account::ID()
{
	return _accountID;
}

void Account::ID(int value)
{
	if (value < 1)
	{
		_accountID = rand();
		printf("\nWarning: Account ID can't be less than one\nNew Account ID is %d", _accountID);
		//return -1;
	}
	else
		_accountID = value;
	//return 1;
}

int Account::Sum()
{
	return _accountSum;
}

void Account::Sum(int value)
{
	if ((_accountSum + value) < 0)
	{
		//return -1;
	}
	else
		_accountSum += value;
	//return 1;
}


Account::Account()
{
	_accountID = 0;
	_accountSum = 0;
	/*ID(0);
	Sum(0);*/
}


Account::Account(int id, int sum)
{
	ID(id);
	Sum(sum);
}


Account::~Account()
{
}
