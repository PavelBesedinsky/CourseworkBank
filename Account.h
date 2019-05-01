#pragma once
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
class Account
{
private:
	int _accountID;			// Идентификатор аккаунта
	int _accountSum;		// Счет аккаунта
	char _accountName[256];	// Имя владельца

public:
	int ID();				// Функция получения идентификатора аккаунта
	int ID(int value);		// Функция установка идентификатора аккаунта
	int Sum();				// Функция получения счета аккаунта
	int Sum(int value);		// Функция установка счета аккаунта
	Account(int id, int sum);
	~Account();
};

