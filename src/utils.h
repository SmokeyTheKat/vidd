#ifndef __vidd_utils_h__
#define __vidd_utils_h__

#define IS_NUMBER(c) (c >= '0' && c <= '9')
#define IS_LETTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
#define CLAMP(v, m) ((v > m) ? m : v)
#define ABS(x) (((x) >= 0) ? (x) : -(x))

char cstring_is_number(char* cstr)
{
	while (*cstr)
	{
		if (!IS_NUMBER(*cstr)) return 0;
		cstr++;
	}
	return 1;
}
int number_length(long num)
{
	if (num <= 9) return 1;
	else if (num <= 99) return 2;
	else if (num <= 999) return 3;
	else if (num <= 9999) return 4;
	else if (num <= 99999) return 5;
	else if (num <= 999999) return 6;
	else if (num <= 9999999) return 7;
	else if (num <= 99999999) return 8;
	else if (num <= 999999999) return 9;
	else if (num <= 9999999999) return 10;
	else if (num <= 99999999999) return 11;
	else if (num <= 999999999999) return 12;
	else return 13;
}

#endif
