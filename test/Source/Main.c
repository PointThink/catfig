#include <stdio.h>
#include <stdlib.h>

#include "Catfig.h"

int main(int argc, char** argv)
{
	Catfig_Token* tokens;
	uint32_t tokenCount;

	// read the entire file
	FILE* configFile = fopen("TestFile.cf", "rb");
	fseek(configFile, 0L, SEEK_END);
	long fileSize = ftell(configFile);
	rewind(configFile);

	char* string = malloc(fileSize + 1);
	fread(string, 1, fileSize, configFile);
	fclose(configFile);

	string[fileSize] = 0;

	Catfig_Error errorCode = Catfig_Tokenizer_Tokenize(string, fileSize, &tokens, &tokenCount);

	if (errorCode != CATFIG_SUCCESS)
		return errorCode;


	printf("tokenCount: %d\n", tokenCount);

	for (int i = 0; i < tokenCount; i++)
	{
		printf("Type: %d\nValue: %s\n\n", tokens[i].type, tokens[i].value);
	}

	free(tokens);
	free(string);
}
