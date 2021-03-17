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

#endif
