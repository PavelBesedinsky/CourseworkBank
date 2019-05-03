#pragma once
struct SQuery
{
	int _qRequest;			// Kind of query
	int _qClientID;		// Client ID
	int _qSum;			// Sum of query
	int _qResult;		// Result of query
	char _qText[100];	// Text of answer
						// char '0' means end of the string
};
