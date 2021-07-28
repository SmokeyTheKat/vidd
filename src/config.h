#ifndef __CONFIG_H__
#define __CONFIG_H__

#define KEY_CTRL(c) ((int)(c-96))

#define VIDD_MULTI_KEY_BIND_START(n, k) \
	void n (struct vidd_client* client) \
	{ \
		uint32_t nkey = getch(); \
		switch (nkey) \
		{
#define VIDD_MULTI_KEY_BIND_OPTION(k, f) \
		case k: \
		{ \
			f(client); \
		} break;
#define VIDD_MULTI_KEY_BIND_OPTION_DEFAULT(f) \
		default: \
		{ \
			f(client, nkey); \
		} break;
#define VIDD_MULTI_KEY_BIND_END(n) \
		} \
	}

#include "./frame.h"

void vidd_test(struct vidd_client* client)
{
	struct frame frame = { 10, 5, 150, 30, { 0 }, 0, 0 };
	frame.buffer = make_buffer(150);
	char line[2048] = {0};
	FILE* fp = popen("/bin/make 2>&1", "r");
	while (fgets(line, sizeof(line), fp))
	{
		buffer_push_cstring(&frame.buffer, line, strlen(line));
	}
	frame_draw(&frame);
}

VIDD_MULTI_KEY_BIND_START(vidd_delete_object_multi_key, 'a')
	VIDD_MULTI_KEY_BIND_OPTION('w', vidd_delete_word)
VIDD_MULTI_KEY_BIND_END(vidd_delete_object_multi_key)

VIDD_MULTI_KEY_BIND_START(vidd_multi_key_delete, 'd')
	VIDD_MULTI_KEY_BIND_OPTION('d', vidd_delete_line)
	VIDD_MULTI_KEY_BIND_OPTION('j', vidd_delete_two_lines_down)
	VIDD_MULTI_KEY_BIND_OPTION('k', vidd_delete_two_lines_up)
	VIDD_MULTI_KEY_BIND_OPTION('a', vidd_delete_object_multi_key)
	VIDD_MULTI_KEY_BIND_OPTION_DEFAULT(vidd_delete_movement)
VIDD_MULTI_KEY_BIND_END(vidd_multi_key_delete)

VIDD_MULTI_KEY_BIND_START(vidd_multi_key_copy, 'y')
	VIDD_MULTI_KEY_BIND_OPTION('y', vidd_copy_line)
VIDD_MULTI_KEY_BIND_END(vidd_multi_key_copy)

void vidd_multi_key_delete_then_insert(struct vidd_client* client)
{
	vidd_multi_key_delete(client);
	vidd_enter_insert_mode(client);
}

void(*vidd_normal_mode_keybinds[])(struct vidd_client*) = {
	[0 ... 510]=vidd_void,
	[KEY_CTRL('c')]=vidd_quit_current,
	[':']=vidd_enter_command_mode,
	['/']=vidd_enter_find_next_mode,
	['?']=vidd_enter_find_prev_mode,
	['>']=vidd_indent,
	['<']=vidd_deindent,
	['v']=vidd_enter_select_mode,
	['V']=vidd_enter_line_select_mode,
	['i']=vidd_enter_insert_mode,
	['a']=vidd_enter_insert_mode_right,
	['o']=vidd_insert_on_new_line,
	['O']=vidd_insert_on_new_line_up,
	[KEY_CTRL('o')]=vidd_new_line,
	['x']=vidd_delete,
	[KEY_DELETE]=vidd_delete,
	['X']=vidd_backspace,
	['c']=vidd_multi_key_delete_then_insert,
	['d']=vidd_multi_key_delete,
	['D']=vidd_delete_line,
	['r']=vidd_replace_character,
	['R']=vidd_enter_replace_mode,
	['w']=vidd_move_next_word,
	['e']=vidd_move_next_word_end,
	['b']=vidd_move_prev_word,
	['f']=vidd_find_next_char,
	['F']=vidd_find_prev_char,

	[KEY_LEFT]=vidd_move_left,
	[KEY_DOWN]=vidd_move_down,
	[KEY_UP]=vidd_move_up,
	[KEY_RIGHT]=vidd_move_right,
	['h']=vidd_move_left,
	['j']=vidd_move_down,
	['k']=vidd_move_up,
	['l']=vidd_move_right,

	['H']=vidd_view_move_left,
	['L']=vidd_view_move_right,
	['K']=vidd_view_move_up,
	['J']=vidd_view_move_down,
	['n']=vidd_frame_move_right,
	['M']=vidd_set_marker,
	['m']=vidd_goto_marker,
	['z']=vidd_view_center,
	['Z']=vidd_cursor_center,
	['g']=vidd_goto_top,
	['G']=vidd_goto_bottom,
	['y']=vidd_multi_key_copy,
	['p']=vidd_paste,
	['$']=vidd_move_to_line_end,
	['0']=vidd_move_to_line_start,
	['^']=vidd_move_to_line_start,
	[KEY_RETURN]=vidd_swap,
	[KEY_CTRL_BACKSLASH]=vidd_duplicate,
	[KEY_CTRL('j')]=vidd_client_next,
	[KEY_CTRL('k')]=vidd_client_prev,
	[KEY_CTRL('h')]=vidd_decrease_master_size,
	[KEY_CTRL('l')]=vidd_increase_master_size,
	[27]=vidd_enter_normal_mode,
	[255]=vidd_toggle_drawing,
	[443]=vidd_toggle_drawing,
	['~']=vidd_test,
};

void(*vidd_select_mode_keybinds[])(struct vidd_client*) = {
	[0 ... 510]=0,
	['x']=vidd_delete_selection,
	['d']=vidd_delete_selection,
	['y']=vidd_selection_copy,
	['s']=vidd_selection_swap_cursor,
};

void(*vidd_insert_mode_keybinds[])(struct vidd_client*) = {
	[0 ... 510]=vidd_insert_char,
	[KEY_CTRL('c')]=vidd_enter_normal_mode,
	[KEY_BACKSPACE]=vidd_backspace,
	[KEY_BACKSPACE]=vidd_backspace,
	[KEY_DELETE]=vidd_delete,
	[KEY_CTRL_BACKSPACE]=vidd_delete,
};

struct command vidd_commands[] = {
	{ "!f", vidd_run_command_in_frame },
	{ "w", vidd_write },
	{ "q", vidd_client_quit },
	{ "qa", vidd_exit },
	{ "wq", vidd_write_quit },
	{ "e", vidd_edit },
	{ "vs", vidd_vsplit },
	{ "s", vidd_split },
};

#endif
