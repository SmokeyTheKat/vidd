void vidd_goto_bottom(struct client* c)
{
	struct line* l = c->cur.y;
	while (l->next) l = l->next;
	c->spos = l->num-c->height;
	vidd_redraw(*c);
}

void vidd_debug(struct client* c)
{
	cursor_save();
	cursor_move_to(40, 20);
	ddPrintf("spos: %d", c->spos);
	cursor_restore();
}

void(*mode_normal_functions[])(struct client*) = {
	['q']=vidd_exit,
	['h']=vidd_move_left,
	['j']=vidd_move_down,
	['k']=vidd_move_up,
	['l']=vidd_move_right,
	['x']=vidd_delete,
	['X']=vidd_backspace,
	['i']=vidd_enter_insert_mode,
	['a']=vidd_right_insert,
	['r']=vidd_enter_replace_mode,
	['o']=vidd_insert_line_down,
	['O']=vidd_insert_line_up,
	['d']=vidd_delete_commands,
	['y']=vidd_copy_line,
	['p']=vidd_paste_line,
	['w']=vidd_skip_word,
	['G']=vidd_goto_bottom,
	['$']=vidd_goto_end,
	['^']=vidd_goto_start,
	['~']=vidd_debug,
};
