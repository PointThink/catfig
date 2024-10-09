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

	uint32_t currentIndex = 0;

	TokenArray tokenArray = TokenArray_Init();

	while (currentIndex < stringLength)
	{
		if (!parsingString)
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
			else if (string[currentIndex] == '"')
				parsingString = true;
		}
		else
		{
			bool stringEscapeNextChar = false;
			uint32_t parsedStringBegin = 0;
			uint32_t parsedStringLength = 0;

			parsedStringBegin = currentIndex;

			// first count the number of chars in the string
			while (string[currentIndex] != '"')
			{
				if (string[currentIndex] == '\\')
					stringEscapeNextChar = true;
				else
					parsedStringLength++;

				currentIndex++;
			}

			char* parsedString = malloc(parsedStringBegin + parsedStringLength + 1);

			currentIndex = parsedStringBegin;
			int stringIndex = 0;

			while (string[currentIndex] != '"')
			{
				if (string[currentIndex] == '\\')
					stringEscapeNextChar = true;
				else
				{
					parsedString[stringIndex] = string[currentIndex];
					stringIndex++;
				}

				currentIndex++;
			}

			parsedString[stringIndex] = '\0';

			TokenArray_Append(&tokenArray, CreateToken(CATFIG_TOKENTYPE_VALUE_STRING, parsedString, strlen(parsedString), parsedStringBegin, currentIndex));
			parsingString = false;
		}

		currentIndex++;
	}

	(*outTokenArray) = tokenArray.tokens;

	return CATFIG_SUCCESS;
}