#include <ctype.h>

#include "Catfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
	Catfig_Token* tokens;
	uint32_t count;
}
TokenArray;

TokenArray TokenArray_Init()
{
	TokenArray array;
	array.tokens = calloc(0, sizeof(Catfig_Token));
	array.count = 0;

	return array;
}

void TokenArray_Append(TokenArray* array, Catfig_Token token)
{
	array->count++;
	array->tokens = realloc(array->tokens, array->count * sizeof(Catfig_Token));
	array->tokens[array->count - 1] = token;
}

Catfig_Token CreateToken(Catfig_TokenType type, char const *text, uint32_t valueLength, uint32_t begin, uint32_t end)
{
	Catfig_Token token;
	token.type = type;
	token.value = text;
	token.valueLength = valueLength;
	token.charRangeBegin = begin;
	token.charRangeEnd = end;

	return token;
}

Catfig_Error Catfig_Tokenizer_Tokenize(char* string, uint32_t stringLength, Catfig_Token** outTokenArray, uint32_t* outTokenCount)
{
	(*outTokenArray) = malloc(sizeof(Catfig_Token));
	(*outTokenCount) = 0;

	bool parsingString = false;
	bool parsingName = false;
	bool parsingNumber = false;

	uint32_t currentIndex = 0;

	TokenArray tokenArray = TokenArray_Init();

	while (currentIndex < stringLength)
	{
		if (parsingString)
		{
			bool stringEscapeNextChar = false;
			uint32_t parsedStringBegin = 0;
			uint32_t parsedStringLength = 0;

			parsedStringBegin = currentIndex;

			// first count the number of chars in the string
			while (true)
			{
				if (string[currentIndex] == '"' && !stringEscapeNextChar)
					break;
				if (string[currentIndex] == '\\'  && !stringEscapeNextChar)
				{
					currentIndex++;
					stringEscapeNextChar = true;
				}
				else
				{
					parsedStringLength++;
					currentIndex++;
					stringEscapeNextChar = false;
				}
			}

			char* parsedString = malloc(parsedStringLength + 1);

			currentIndex = parsedStringBegin;
			int stringIndex = 0;

			// first count the number of chars in the string
			while (true)
			{
				if (string[currentIndex] == '"' && !stringEscapeNextChar)
					break;
				if (string[currentIndex] == '\\'  && !stringEscapeNextChar)
				{
					currentIndex++;
					stringEscapeNextChar = true;
				}
				else
				{
					parsedString[stringIndex] = string[currentIndex];
					stringIndex++;
					currentIndex++;
					stringEscapeNextChar = false;
				}
			}

			parsedString[stringIndex] = '\0';

			TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_VALUE_STRING, parsedString, strlen(parsedString), parsedStringBegin, currentIndex));
			parsingString = false;
			*outTokenCount += 1;
		}
		else if (parsingName)
		{
			currentIndex--; // the loop increments the currentIndex after detecting the first char of a name so we have to decrement it
			uint32_t nameBegin = currentIndex;
			uint32_t nameLength = 0;

			while (isalnum(string[currentIndex]) || string[currentIndex] == '_')
			{
				currentIndex++;
				nameLength++;
			}

			char* nameString = malloc(nameLength + 1);
			nameString[nameLength] = '\0';

			currentIndex = nameBegin;
			uint32_t nameIndex = 0;

			while (nameIndex < nameLength)
			{
				nameString[nameIndex] = string[currentIndex];
				currentIndex++;
				nameIndex++;
			}

			parsingName = false;

			Catfig_TokenType tokenType = CATFIG_TOKENTYPE_NAME;

			if (strcmp(nameString, "true") == 0)
				tokenType = CATFIG_TOKENTYPE_VALUE_TRUE;
			else if (strcmp(nameString, "false") == 0)
				tokenType = CATFIG_TOKENTYPE_VALUE_FALSE;
			else if (strcmp(nameString, "null") == 0)
				tokenType = CATFIG_TOKENTYPE_VALUE_NULL;

			TokenArray_Append(&tokenArray, CreateToken(tokenType, nameString, nameLength, nameBegin, nameBegin + nameLength));
			*outTokenCount += 1;
		}
		else if (parsingNumber)
		{
			currentIndex--; // the loop increments the currentIndex after detecting the first char of a name so we have to decrement it
			uint32_t numberBegin = currentIndex;
			uint32_t numberLength = 0;

			while (isdigit(string[currentIndex]) || string[currentIndex] == '.')
			{
				currentIndex++;
				numberLength++;
			}

			char* numberString = malloc(numberLength + 1);
			numberString[numberLength] = '\0';

			uint32_t numberIndex = 0;
			currentIndex = numberBegin;

			while (numberIndex < numberLength)
			{
				numberString[numberIndex] = string[currentIndex];
				numberIndex++;
				currentIndex++;
			}

			TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_VALUE_NUMBER, numberString, numberLength, numberBegin, numberBegin + numberLength));
			parsingNumber = false;
			*outTokenCount += 1;
		}
		else
		{
			if (string[currentIndex] == '{')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_BRACE_LEFT, "{", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == '}')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_BRACE_RIGHT, "}", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == '[')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_SQUARE_BRACKET_LEFT, "[", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == ']')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_SQUARE_BRACKET_RIGHT, "]", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == ',')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_COMMA, ",", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == ';')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_SEMICOLON, ";", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == '=')
			{
				TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_EQUALS, "=", 1, currentIndex, currentIndex));
				*outTokenCount += 1;
			}
			else if (string[currentIndex] == '"' || string[currentIndex] == '_')
				parsingString = true;
			else if (isalpha(string[currentIndex]))
				parsingName = true;
			else if (isdigit(string[currentIndex]) || string[currentIndex] == '.')
				parsingNumber = true;
		}

		currentIndex++;
	}

	(*outTokenArray) = tokenArray.tokens;

	return CATFIG_SUCCESS;
}