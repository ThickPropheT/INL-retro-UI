#include "str_extensions.h"

char *strrpl(char *str, char oldC, char newC)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == oldC)
			str[i] = newC;
	}
	
	return str;
}

char *strmarsh(char *str)
{
	char *result = CoTaskMemAlloc(strlen(str) + 1);
	strcpy(result, str);
	
	return result;
}

char *strmarshc(const char *str)
{
	char *result = CoTaskMemAlloc(strlen(str) + 1);
	strcpy(result, str);
	
	return result;
}