#pragma once

struct Query
{
	int _qID;			// Kind of query
	int _qSum;			// Sum of query
	int _qResult;		// Result of query
	char _qText[20];	// Text of answer
						// char '0' means end of the string
};