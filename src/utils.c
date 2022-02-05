#include <vidd/utils.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

char cstring_is_n_number(char* cstr, intmax_t n)
{
	intmax_t i;
	for (i = 0; cstr[i] && i < n; i++)
		if (!IS_NUMBER(cstr[i]))
			return false;
	return i == n;
}

char* strlstr(char* src, char* find, intmax_t src_length)
{
	intmax_t find_length = strlen(find);
	for (intmax_t i = src_length - 1; i >= 0; i--)
		if (!strncmp(&src[i], find, find_length)) return &src[i];
	return 0;
}

int visable_strlen(char* str)
{
	int max_length = strlen(str);
	int length = 0;
	char* ptr = str;

	mblen(0, 0);

	while (*ptr)
	{
		if (memcmp(ptr, "\x1b[", 2) == 0)
		{
			while (*(ptr++) != 'm');
		}
		else
		{
			int chr_length = mblen(ptr, max_length);
			if (chr_length == -1)
				return 0;
			ptr += chr_length;
			length++;
		}
	}

	return length;
}

bool string_includes_list(char* str, char* _list)
{
	char* list = malloc(strlen(_list) + 1);
	strcpy(list, _list);

	char* word = strtok(list, " \n\0");
	if (word == 0)
	{
		free(list);
		return true;
	}

	do
	{
		if (!strstr(str, word))
		{
			free(list);
			return false;
		}
	} while((word = strtok(0, " \n\0")));

	free(list);
	return true;
}

void set_terminal_title(char* title)
{
	printf("\x1b]0;%s\x07", title);
}

intmax_t number_get_length(intmax_t num)
{
	switch (num)
	{
		case 0 ... 9: return 1;
		case 10 ... 99: return 2;
		case 100 ... 999: return 3;
		case 1000 ... 9999: return 4;
		case 10000 ... 99999: return 5;
		case 100000 ... 999999: return 6;
		case 1000000 ... 9999999: return 7;
		case 10000000 ... 99999999: return 8;
		case 100000000 ... 999999999: return 9;
		case 1000000000 ... 9999999999: return 10;
		case 10000000000 ... 99999999999: return 11;
		case 100000000000 ... 999999999999: return 12;
		case 1000000000000 ... 9999999999999: return 13;
		case 10000000000000 ... 99999999999999: return 14;
		case 100000000000000 ... 999999999999999: return 15;
		case 1000000000000000 ... 9999999999999999: return 16;
		case 10000000000000000 ... 99999999999999999: return 17;
		case 100000000000000000 ... 999999999999999999: return 18;
	}
	return 0;
}

void screen_get_size(intmax_t* width, intmax_t* height)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	*width = w.ws_col;
	*height = w.ws_row;
}

void screen_save(void)
{
	system("tput smcup");
}
void screen_restore(void)
{
	system("tput rmcup");
}


void screen_clear(void)
{
	printf("\x1b[2J\x1b[1;1H");
}

void cursor_move(int x, int y)
{
	if (y != 0)
	{
		if (y < 0) printf("\x1b[%dA", -1*y);
		else printf("\x1b[%dB", y);
	}
	if (x != 0)
	{
		if (x < 0) printf("\x1b[%dD", -1*x);
		else printf("\x1b[%dC", x);
	}
}
void cursor_move_to(int x, int y)
{
	printf("\x1b[%d;%dH", y+1, x+1);
}
void cursor_return(void)
{
	printf("\r");
}
void cursor_erase_line(void)
{
	printf("\x1b[2K");
}
void cursor_save(void)
{
	printf("\x1b[s");
}
void cursor_restore(void)
{
	printf("\x1b[u");
}
void cursor_home(void)
{
	cursor_move_to(0, 0);
}

