#pragma once

#define CATFIG_API extern

#include <stdint.h>

// ================================================
// Errors
// ================================================
typedef enum
{
	CATFIG_SUCCESS = 0,
	CATFIG_TOKENIZER_ERROR_UNEXPECTED_CHAR,
	CATFIG_TOKENIZER_UNCLOSED_STRING,
}
Catfig_Error;

// ================================================
// Tokenizer
// ================================================
typedef enum
{
	CATFIG_TOKENTYPE_UNKNOWN = 0,

	CATFIG_TOKENTYPE_BRACE_LEFT, // {
	CATFIG_TOKENTYPE_BRACE_RIGHT, // }
	CATFIG_TOKENTYPE_SQUARE_BRACKET_LEFT, // [
	CATFIG_TOKENTYPE_SQUARE_BRACKET_RIGHT, // ]

	CATFIG_TOKENTYPE_COMMA, // ,
	CATFIG_TOKENTYPE_SEMICOLON, // ;
	CATFIG_TOKENTYPE_EQUALS, // =

	CATFIG_TOKENTYPE_NAME,
	CATFIG_TOKENTYPE_VALUE_NUMBER,
	CATFIG_TOKENTYPE_VALUE_STRING,
	CATFIG_TOKENTYPE_VALUE_TRUE,
	CATFIG_TOKENTYPE_VALUE_FALSE,
	CATFIG_TOKENTYPE_VALUE_NULL,
}
Catfig_TokenType;

typedef struct
{
	Catfig_TokenType type;

	uint32_t valueLength;
	char* value;

	uint32_t charRangeBegin; // where in the tokenized string the token starts
	uint32_t charRangeEnd; // where it ends
}
Catfig_Token;

/*
Converts the given string into an array of tokens

Params:
	- string: The string to be tokenized
	- stringLength: The length of the string without terminator

Returns:
	- Return value: An error info struct
	- outTokenArray: The finalized array of tokens
	- outTokenCount: How many tokens have been created
	
*/
CATFIG_API Catfig_Error Catfig_Tokenizer_Tokenize(char* string, uint32_t stringLength, Catfig_Token** outTokenArray, uint32_t* outTokenCount);