#include <vidd/syntax.h>

#include <vidd/vidd.h>
#include <vidd/themes.h>
#include <vidd/style.h>

#include <stdlib.h>

static char* current_color = "";

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line);
static void vidd_syntax_push_next(struct buffer* buffer, struct line* line, intmax_t* i, bool colored);
static bool process_range_syntaxes(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* i);
static bool is_in_middle_of_word(char* text, intmax_t i);
static bool current_word_is_number(char* text, intmax_t i);
static bool current_word_is_function_call(struct vidd_client* client, struct line* line, intmax_t i);
static void push_whole_line_without_syntax(struct vidd_client* client, struct buffer* buffer, struct line* line);
static void push_number(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i);
static void push_function_call(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i);
static bool push_word(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i);

void vidd_syntax_ftdetect_cstring(struct vidd_client* client, char* rfile_name)
{
	struct buffer bfile_name = make_buffer_from_cstring(rfile_name);
	buffer_insert_at(&bfile_name, '.', 0);
	char* file_name = bfile_name.data;
	for (intmax_t i = 0; i < syntaxes_length; i++)
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

static void vidd_syntax_push_next(struct buffer* buffer, struct line* line, intmax_t* i, bool colored)
{
	if (!IS_PRINTABLE(line->buffer.data[*i]))
	{
		mblen(0, 0);
		int charlen = mblen(&line->buffer.data[*i], line->buffer.length - (*i));
		if (line->buffer.data[*i] == '\x1b' || charlen == -1)
		{
			if (colored) buffer_print(buffer, FRGB("100", "100", "100") "?" NOSTYLE);
			else
			{
				buffer_print(buffer, FRGB("100", "100", "100") "?" NOSTYLE);
				buffer_print(buffer, current_color);
			}
			(*i)++;
		}
		else
		{
			for (int j = 0; j < charlen; j++)
				buffer_push(buffer, line->buffer.data[(*i)++]);
			for (int j = 0; j < charlen-1; j++)
			{
				if (colored) buffer_print(buffer, FRGB("100", "100", "100") "?" NOSTYLE);
				else
				{
					buffer_print(buffer, FRGB("100", "100", "100") "?" NOSTYLE);
					buffer_print(buffer, current_color);
				}
			}
		}
	}
	else buffer_push(buffer, line->buffer.data[(*i)++]);
}

static bool process_range_syntaxes(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i)
{
	intmax_t i = *_i;
	char* text = line->buffer.data;
	for (intmax_t j = 1+(intmax_t)client->syntax[0]; client->syntax[j]; j++)
	{
		if (client->syntax[j][0] == SCID_RANGE
			|| client->syntax[j][0] == SCID_RANGE_NI)
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
	
				current_color = active_theme->syntax_styles[(int)stype];
				buffer_print(buffer, active_theme->syntax_styles[(int)stype]);
				buffer_push_cstring(buffer, &text[i], key_start_length);
	
				i += key_start_length;
	
				if (key_end[0] == '\0') key_end_length = 1;
				while (i < line->buffer.length && i < client->view.x + client->view.width && strncmp(&text[i], key_end, key_end_length))
					vidd_syntax_push_next(buffer, line, &i, false);

				if (client->syntax[j][0] != SCID_RANGE_NI && i < line->buffer.length && i < client->view.x + client->view.width && key_end[0] != '\0')
				{
					buffer_push_cstring(buffer, &text[i], key_end_length);
					i += key_end_length;
				}
				buffer_push_default_style(buffer);
				*_i = i;
				return true;
			}
		}
	}
	*_i = i;
	return false;
}

static bool is_in_middle_of_word(char* text, intmax_t i)
{
	return i > 0 && IS_CHARACTER(text[i]) && IS_CHARACTER(text[i-1]);
}

static bool current_word_is_number(char* text, intmax_t i)
{
	return IS_NUMBER(text[i]) && (i == 0 || !IS_CHARACTER(text[i-1]));
}

static bool current_word_is_function_call(struct vidd_client* client, struct line* line, intmax_t i)
{
	char* text = line->buffer.data;

	if (!IS_CHARACTER(text[i]))
		return false;
	if (i > 0 && IS_CHARACTER(text[i-1]))
		return false;

	while (i < line->buffer.length && i < client->view.x + client->view.width && IS_CHARACTER(text[i])) i++;

	return text[i] == '(';
}

static void push_whole_line_without_syntax(struct vidd_client* client, struct buffer* buffer, struct line* line)
{
	for (intmax_t i = client->view.x; i < line->buffer.length && i < client->view.x + client->view.width;)
		vidd_syntax_push_next(buffer, line, &i, true);
}

static void push_number(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i)
{
	char* text = line->buffer.data;

	intmax_t i = *_i;
	buffer_print(buffer, active_theme->syntax_styles[SCID_NUMBER]);
	while (i < line->buffer.length && i < client->view.x + client->view.width && IS_NUMBER(text[i])) buffer_push(buffer, line->buffer.data[i++]);
	buffer_push_default_style(buffer);
	*_i = i;
}

static void push_function_call(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i)
{
	char* text = line->buffer.data;

	intmax_t i = *_i;
	buffer_print(buffer, active_theme->syntax_styles[SCID_FUNCTION]);
	while (i < line->buffer.length && i < client->view.x + client->view.width && IS_CHARACTER(text[i])) buffer_push(buffer, line->buffer.data[i++]);
	buffer_push_default_style(buffer);
	*_i = i;
}

static bool push_word(struct vidd_client* client, struct buffer* buffer, struct line* line, intmax_t* _i)
{
	char* text = line->buffer.data;
	intmax_t i = *_i;

	intmax_t word_length = strspn(&text[i], CSET_CHARACTERS);
	if (word_length == 0) word_length = 1;
	for (intmax_t j = 1+(intmax_t)client->syntax[0]; client->syntax[j]; j++)
	{
		if (strlen(client->syntax[j]+1) == word_length && !strncmp(&text[i], client->syntax[j]+1, word_length))
		{
			buffer_print(buffer, active_theme->syntax_styles[(int)client->syntax[j][0]]);
			buffer_push_cstring(buffer, &text[i], MIN(word_length, client->view.x + client->view.width - i));
			buffer_push_default_style(buffer);
			i += word_length;
			*_i = i;
			return true;
		}
	}
	*_i = i;
	return false;
}

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line)
{
	char* text = line->buffer.data;

	if (!client->syntaxOn)
	{
		push_whole_line_without_syntax(client, buffer, line);
		return;
	}

	for (intmax_t i = client->view.x; i < line->buffer.length && i < client->view.x + client->view.width;)
	{
		if (current_word_is_number(text, i))
		{
			push_number(client, buffer, line, &i);
			continue;
		}

		if (current_word_is_function_call(client, line, i))
		{
			push_function_call(client, buffer, line, &i);
			continue;
		}

		if (process_range_syntaxes(client, buffer, line, &i))
			continue;

		if (is_in_middle_of_word(text, i))
		{
			buffer_push(buffer, line->buffer.data[i++]);
			continue;
		}

		if (push_word(client, buffer, line, &i))
			continue;
		
		vidd_syntax_push_next(buffer, line, &i, true);
	}
}
