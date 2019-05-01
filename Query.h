#pragma once
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
// Запросы:
// 0 - Положить деньги
// 1 - Снять деньги
// 2 - Посмотреть баланс
// Ответы:
// 3 - Запрос выполнен
// 4 - Запрос не выполненфы
class Query
{
public:
	int _qID;
	int _qSum;
	int _qResult;
	char _qText[256];
	Query(int ID, int Sum, int Result, char* Text);
	~Query();
};

