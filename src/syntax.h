//#ifndef __VIDD_SYNTAX_H__
//#define __VIDD_SYNTAX_H__

#include "vidd.h"
#include "line.h"
#include "buffer.h"

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line);

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
			for (intmax_t j = 0; client->syntax[j]; j++)
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

						buffer_push_cstring(buffer, SYNTAX_COLORS[(int)stype], strlen(SYNTAX_COLORS[(int)stype]));
						buffer_push_cstring(buffer, &text[i], key_start_length);

						i += key_start_length;

						if (key_end[0] == '\0') key_end_length = 1;
						while (i < line->buffer.length && i < client->view.x + client->view.width && strncmp(&text[i], key_end, key_end_length))
						{
							if (!IS_PRINTABLE(line->buffer.data[i])) { buffer_push_cstring(buffer, FRGB("100", "100", "100") "?" NOSTYLE, sizeof(FRGB("100", "100", "100") "?" NOSTYLE)-1); i++; }
							else buffer_push(buffer, line->buffer.data[i++]);
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
			for (intmax_t j = 0; client->syntax[j]; j++)
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
			if (!IS_PRINTABLE(line->buffer.data[i])) { buffer_push_cstring(buffer, FRGB("100", "100", "100") "?" NOSTYLE, sizeof(FRGB("100", "100", "100") "?" NOSTYLE)-1); i++; }
			else buffer_push(buffer, line->buffer.data[i++]);
VIDD_SYNTAX_APPLY_TO_BUFFER_TEXT_LOOP:
		}
	}
	else
	{
		for (intmax_t i = client->view.x; i < line->buffer.length && i < client->view.x + client->view.width;)
		{
			if (!IS_PRINTABLE(line->buffer.data[i])) { buffer_push_cstring(buffer, FRGB("100", "100", "100") "?" NOSTYLE, sizeof(FRGB("100", "100", "100") "?" NOSTYLE)-1); i++; }
			else buffer_push(buffer, line->buffer.data[i++]);
		}
	}
}

//#endif
