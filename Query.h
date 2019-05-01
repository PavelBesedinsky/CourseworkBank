#pragma once
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
// �������:
// 0 - �������� ������
// 1 - ����� ������
// 2 - ���������� ������
// ������:
// 3 - ������ ��������
// 4 - ������ �� ����������
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

