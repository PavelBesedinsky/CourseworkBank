#include "Query.h"



Query::Query()
{
	_qID		= 0;
	_qSum		= 0;
	_qResult	= 0;
	_qText		= "";
}

Query::~Query()
{
}

int Query::ID()
{
	return _qID;
}

void Query::ID(int value)
{
	_qID = value;
}

int Query::Sum()
{
	return _qSum;
}

void Query::Sum(int value)
{
	_qSum = value;
}

int Query::Result()
{
	return _qResult;
}

void Query::Result(int value)
{
	if (IsMistake(value))
		_qResult = value;
	else
		_qResult = 4;
}

int Query::TextLength()
{
	return _qText.length();
}

std::string Query::Text()
{
	return _qText;
}

void Query::Text(std::string string)
{
	_qText = string;
}

bool Query::IsMistake(int value)
{
	switch (value)
	{
	case 0:
		return true;
	case 1:
		return true;
	case 2:
		return true;
	case 3:
		return true;
	case 4:
		return true;
	default:
		return false;
	}
	return false;
}
