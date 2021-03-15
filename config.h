#include "styles.h"

#define MESSAGE_SLEEP_TIME 400000

void vidd_debug(struct client* c)
{
}

struct command commands[] = {
	{ "w", vidd_write },
	{ "q", vidd_exit },
	{ "q!", vidd_force_exit },
};

void(*mode_normal_functions[])(struct client*) = {
	[0 ... 1000]=vidd_void,
	['0' ... '9']=vidd_read_number,
	['!']=vidd_read_number,
	[':']=vidd_read_command,
	['Q']=vidd_exit,
	['q']=vidd_start_macro,
	['@']=vidd_run_macro,
	['h']=vidd_move_left,
	['j']=vidd_move_down,
	['k']=vidd_move_up,
	['l']=vidd_move_right,
	['x']=vidd_delete,
	['X']=vidd_backspace,
	['i']=vidd_enter_insert_mode,
	['I']=vidd_insert_at_start,
	['a']=vidd_right_insert,
	['A']=vidd_insert_at_end,
	['r']=vidd_replace_once,
	['R']=vidd_enter_replace_mode,
	['o']=vidd_insert_line_down,
	['O']=vidd_insert_line_up,
	['d']=vidd_delete_commands,
	['D']=vidd_delete_line,
	['y']=vidd_copy,
	['Y']=vidd_copy_line,
	['v']=vidd_enter_select_mode,
	['V']=vidd_enter_line_select_mode,
	['p']=vidd_paste_line,
	['w']=vidd_skip_word,
	['b']=vidd_skip_word_back,
	['g']=vidd_goto_top,
	['G']=vidd_goto_bottom,
	['z']=vidd_center,
	['<']=vidd_deindent,
	['>']=vidd_indent,
	['$']=vidd_goto_end,
	['^']=vidd_goto_start,
	['~']=vidd_void,
	[27]=vidd_enter_normal_mode,
};
