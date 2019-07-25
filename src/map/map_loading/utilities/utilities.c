// Liam Wynn, 7/20/2019, Spaceman Engine Level Loading

#include "./utilities.h"

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

char* remove_quotations_from_string(char* str) {
	if(!str)
		return NULL;

	char* result = NULL;
	int str_len = strlen(str);

	char* str_without_first_quote;
	int result_len;

	if(str_len > 2 && str[0] == '"' && str[str_len - 1] == '"') {
		str_without_first_quote = &str[1];
		result_len = strlen(str_without_first_quote) - 1;

		result = (char*)malloc(sizeof(char) * (result_len + 1));
		strncpy(result, str_without_first_quote, result_len);
		result[result_len] = '\0';
	}

	return result;
}
