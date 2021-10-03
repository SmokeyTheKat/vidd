//#ifndef __VIDD_SYNTAX_H__
//#define __VIDD_SYNTAX_H__

#include "vidd.h"
#include "line.h"
#include "buffer.h"

char* current_color = "";

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line);
void vidd_syntax_push_next(struct buffer* buffer, struct line* line, intmax_t* i, bool colored);

void vidd_syntax_ftdetect_cstring(struct vidd_client* client, char* rfile_name)
{
	struct buffer bfile_name = make_buffer_from_cstring(rfile_name);
	buffer_insert_at(&bfile_name, '.', 0);
	char* file_name = bfile_name.data;
	for (intmax_t i = 0; i < sizeof(syntaxes) / sizeof(char**); i++)
	{
		char** syntax = syntaxes[i];
		for (intmax_t j = 1; j < 1+(intmax_t)syntax[0]; j++)
		{
			char* ext = syntax[j];
			if (!strcmp(&file_name[strlen(file_name)-strlen(ext)], ext))
			{
				client->syntaxOn = true;
				client->syntax = syntaxes[i];
				return;
			}
		}
	}
	client->syntaxOn = false;
	free_buffer(&bfile_name);
}
void vidd_syntax_ftdetect(struct vidd_client* client)
{
	vidd_syntax_ftdetect_cstring(client, client->file_name.data);
}

void vidd_syntax_push_next(struct buffer* buffer, struct line* line, intmax_t* i, bool colored)
{
/*
	if (!IS_PRINTABLE(line->buffer.data[*i]))
	{
		uint8_t val = line->buffer.data[*i];
		int mclen = 0;
		if (val >= 0xc0 && val <= 0xdf) mclen = 2;
		else if (val >= 0xe0 && val <= 0xef) mclen = 3;
		else if (val >= 0xf0 && val <= 0xf7) mclen = 4;

		if (mclen > 1)
		{
			bool printgood = true;
			for (int j = 0; j < mclen; j++)
			{
				if ((*i) + j >= line->buffer.length && IS_PRINTABLE(line->buffer.data[(*i) + j]))
				{
					printgood = false;
					break;
				}
			}
			if (printgood)
			{
				buffer_push_cstring(buffer, &line->buffer.data[(*i)], mclen);
				for (int j = 0; j < mclen-1; j++)
				{
					if (colored) buffer_push_cstring(buffer, FRGB("100", "100", "100") "·" NOSTYLE, sizeof(FRGB("100", "100", "100") "·" NOSTYLE)-1);
					else
					{
						buffer_push_cstring(buffer, FRGB("100", "100", "100") "·" NOSTYLE, sizeof(FRGB("100", "100", "100") "·" NOSTYLE)-1);
						buffer_push_cstring(buffer, current_color, strlen(current_color));
					}
				}
				(*i) += mclen;
			}
			else buffer_push(buffer, line->buffer.data[(*i)++]);
		}
	}
*/
	if (!IS_PRINTABLE(line->buffer.data[*i]))
	{
		if (colored) buffer_push_cstring(buffer, FRGB("100", "100", "100") "?" NOSTYLE, sizeof(FRGB("100", "100", "100") "?" NOSTYLE)-1);
		else
		{
			buffer_push_cstring(buffer, FRGB("100", "100", "100") "?" NOSTYLE, sizeof(FRGB("100", "100", "100") "?" NOSTYLE)-1);
			buffer_push_cstring(buffer, current_color, strlen(current_color));
		}
		(*i)++;
	}
	else buffer_push(buffer, line->buffer.data[(*i)++]);
}

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line)
{
	char* text = line->buffer.data;
	if (client->syntaxOn)
	{
		for (intmax_t i = client->view.x; i < line->buffer.length && i < client->view.x + client->view.width;)
		{
			if (IS_NUMBER(text[i]) && !IS_CHARACTER(text[i-1]))
			{
				buffer_push_cstring(buffer, SYNTAX_NUMBER_COLOR, sizeof(SYNTAX_NUMBER_COLOR)-1);
				while (i < line->buffer.length && i < client->view.x + client->view.width && IS_NUMBER(text[i])) buffer_push(buffer, line->buffer.data[i++]);
				buffer_push_cstring(buffer, NOSTYLE, sizeof(NOSTYLE)-1);
				continue;
			}
			for (intmax_t j = 1+(intmax_t)client->syntax[0]; client->syntax[j]; j++)
			{
				if (client->syntax[j][0] == SCID_RANGE)
				{
					char stype = client->syntax[j][1];
					char* key_start = client->syntax[j]+2;
					intmax_t key_start_length = strlen(key_start);
					if (!strncmp(&text[i], key_start, key_start_length))
					{
						char* key_end = &key_start[key_start_length+1];
						intmax_t key_end_length = strlen(key_end);

						bool exists = false;
						for (intmax_t k = i + key_start_length; text[k]; k++)
						{
							if (!strncmp(&text[k], key_end, key_end_length))
							{
								exists = true;	
								break;
							}
						}
						if (!exists && key_end[0] != '\0') continue;

						current_color = SYNTAX_COLORS[(int)stype];
						buffer_push_cstring(buffer, SYNTAX_COLORS[(int)stype], strlen(SYNTAX_COLORS[(int)stype]));
						buffer_push_cstring(buffer, &text[i], key_start_length);

						i += key_start_length;

						if (key_end[0] == '\0') key_end_length = 1;
						while (i < line->buffer.length && i < client->view.x + client->view.width && strncmp(&text[i], key_end, key_end_length))
						{
							vidd_syntax_push_next(buffer, line, &i, false);
						}
						if (i < line->buffer.length && i < client->view.x + client->view.width && key_end[0] != '\0')
						{
							buffer_push_cstring(buffer, &text[i], key_end_length);
							i += key_end_length;
						}
						buffer_push_cstring(buffer, NOSTYLE, sizeof(NOSTYLE)-1);
						goto VIDD_SYNTAX_APPLY_TO_BUFFER_TEXT_LOOP;
					}
				}
			}

			if (i > 0 && IS_CHARACTER(text[i]) && IS_CHARACTER(text[i-1]))
			{
				buffer_push(buffer, line->buffer.data[i++]);
				continue;
			}
			
			intmax_t word_length = strspn(&text[i], CSET_CHARACTERS);
			if (word_length == 0) word_length = 1;
			for (intmax_t j = 1+(intmax_t)client->syntax[0]; client->syntax[j]; j++)
			{
				if (strlen(client->syntax[j]+1) == word_length && !strncmp(&text[i], client->syntax[j]+1, word_length))
				{
					buffer_push_cstring(buffer, SYNTAX_COLORS[(int)client->syntax[j][0]], strlen(SYNTAX_COLORS[(int)client->syntax[j][0]]));
					buffer_push_cstring(buffer, &text[i], MIN(word_length, client->view.x + client->view.width - i));
					buffer_push_cstring(buffer, NOSTYLE, sizeof(NOSTYLE)-1);
					i += word_length;
					goto VIDD_SYNTAX_APPLY_TO_BUFFER_TEXT_LOOP;
				}
			}
			vidd_syntax_push_next(buffer, line, &i, true);
VIDD_SYNTAX_APPLY_TO_BUFFER_TEXT_LOOP:;
		}
	}
	else
	{
		for (intmax_t i = client->view.x; i < line->buffer.length && i < client->view.x + client->view.width;)
		{
			vidd_syntax_push_next(buffer, line, &i, true);
		}
	}
}

//#endif
