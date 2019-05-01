#pragma once
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <iostream>
// Запросы:
// 0 - Положить деньги
// 1 - Снять деньги
// 2 - Посмотреть баланс
// Ответы:
// 3 - Запрос выполнен
// 4 - Запрос не выполненфы
class Query
{
private:
	int _qID;
	int _qSum;
	int _qResult;
	std::string _qText;
public:
	int ID();
	void ID(int value);
	int Sum();
	void Sum(int value);
	int Result();
	void Result(int value);
	int TextLength();
	std::string Text();
	void Text(std::string string);
	bool IsMistake(int value);

	Query();
	~Query();
};

