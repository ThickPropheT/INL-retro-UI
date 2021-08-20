#pragma once

#include <string.h>
#include <objbase.h>

/// a function to replace all instances of the one specified character with a new character.
/// str		: the string for which to replace characters.
/// oldC	: the character to replace.
/// newC	: the replacement character.
/// returns	: str.	
char *strrpl(char *str, char oldC, char newC);

/// a function to marshall a string for deallocation by a managed (i.e. Windows) environment.
/// str		: the string to marshall.
/// returns	: a copy of the string that can be deallocated by a managed environment.
char *strmarsh(char *str);

/// a function to marshall a const string for deallocation by a managed (i.e. Windows) environment.
/// str		: the const string to marshall.
/// returns	: a copy of the string that can be deallocated by a managed environment.
char *strmarshc(const char *str);