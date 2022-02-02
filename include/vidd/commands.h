#ifndef __VIDD_COMMANDS_H__
#define __VIDD_COMMANDS_H__

#include "vidd.h"

void vidd_redraw_line(struct vidd_client* client);
void vidd_toggle_drawing(struct vidd_client* client);
void vidd_reorganize_clients(struct vidd_client_pool* pool);
void vidd_increase_master_size(struct vidd_client* client);
void vidd_decrease_master_size(struct vidd_client* client);

void vidd_run_line(struct vidd_client* client);

void vidd_floating_window_draw_frame(struct vidd_client* client);
void vidd_floating_increase_height(struct vidd_client* client);
void vidd_floating_decrease_height(struct vidd_client* client);
void vidd_floating_increase_width(struct vidd_client* client);
void vidd_floating_decrease_width(struct vidd_client* client);
void vidd_move_floating_up(struct vidd_client* client);
void vidd_move_floating_down(struct vidd_client* client);
void vidd_move_floating_right(struct vidd_client* client);
void vidd_move_floating_left(struct vidd_client* client);
void vidd_floating_center(struct vidd_client* client);
void vidd_floating_toggle(struct vidd_client* client);

void vidd_check_for_window_size_change(struct vidd_client* client);

void vidd_set_status(struct vidd_client* client);

bool vidd_view_fix_overflow(struct vidd_client* client);
bool vidd_cursor_fix_overflow(struct vidd_client* client);
bool vidd_selection_redraw_needed(struct vidd_client* client);
void vidd_cursor_adjust(struct vidd_client* client, bool can_redraw);
void vidd_text_clear(struct vidd_client* client);

intmax_t vidd_last_movable(struct vidd_client* client);
int vidd_cursor_is_out_of_view(struct vidd_client* client);
intmax_t vidd_view_get_absolute_x_offset(struct vidd_client* client);
intmax_t vidd_view_get_absolute_y_offset(struct vidd_client* client);

void vidd_cursor_center(struct vidd_client* client);

void vidd_move_up(struct vidd_client* client);
void vidd_move_down(struct vidd_client* client);
void vidd_move_right(struct vidd_client* client);
void vidd_move_left(struct vidd_client* client);
void vidd_move_to_line_start(struct vidd_client* client);
void vidd_move_to_line_end(struct vidd_client* client);
void vidd_move_next_word(struct vidd_client* client);
void vidd_move_next_word_end(struct vidd_client* client);
void vidd_move_prev_word(struct vidd_client* client);
void vidd_move_to_spot_in_view_after(struct vidd_client* client, intmax_t pos);

void vidd_view_adjust_offset(struct vidd_client* client);

void vidd_view_center_y(struct vidd_client* client);
void vidd_view_center_x(struct vidd_client* client);
void vidd_view_center(struct vidd_client* client);

void vidd_view_move_right(struct vidd_client* client);
void vidd_view_move_left(struct vidd_client* client);
void vidd_view_move_up(struct vidd_client* client);
void vidd_view_move_down(struct vidd_client* client);

void vidd_set_marker(struct vidd_client* client);
void vidd_goto_marker(struct vidd_client* client);
void vidd_save_spot(struct vidd_client* client);
void vidd_load_spot(struct vidd_client* client);

void vidd_goto_top(struct vidd_client* client);
void vidd_goto_bottom(struct vidd_client* client);
void vidd_move_to(struct vidd_client* client, intmax_t x, intmax_t y);
void vidd_move_to_x(struct vidd_client* client, intmax_t x);

void vidd_macro_toggle(struct vidd_client* client);
void vidd_macro_run(struct vidd_client* client);

void vidd_frame_move_right(struct vidd_client* client);

void vidd_insert_char(struct vidd_client* client);

void vidd_new_line(struct vidd_client* client);
void vidd_insert_on_new_line(struct vidd_client* client);
void vidd_insert_on_new_line_up(struct vidd_client* client);

void vidd_toggle_comment(struct vidd_client* client);

void vidd_indent(struct vidd_client* client);
void vidd_deindent(struct vidd_client* client);

void vidd_split_at_cursor(struct vidd_client* client);

void vidd_man_word(struct vidd_client* client);

void vidd_delete(struct vidd_client* client);
void vidd_delete_word(struct vidd_client* client);
void vidd_delete_movement(struct vidd_client* client, char key);
void vidd_delete_selection(struct vidd_client* client);
void vidd_delete_line(struct vidd_client* client);
void vidd_backspace(struct vidd_client* client);
void vidd_delete_two_lines_up(struct vidd_client* client);
void vidd_delete_two_lines_down(struct vidd_client* client);

void vidd_replace_character(struct vidd_client* client);

void vidd_line_selection_draw(struct vidd_client* client);
void vidd_selection_draw(struct vidd_client* client);
void vidd_selection_indent(struct vidd_client* client);
void vidd_selection_toggle_comment(struct vidd_client* client);
void vidd_selection_deindent(struct vidd_client* client);
void vidd_selection_swap_cursor(struct vidd_client* client);

void vidd_save_copy(void);
void vidd_load_copy(void);
void vidd_copy_to_clipboard(struct vidd_client* client);
void vidd_copy_line(struct vidd_client* client);
void vidd_selection_copy(struct vidd_client* client);

void vidd_paste(struct vidd_client* client);

void vidd_find_next_char(struct vidd_client* client);
void vidd_find_prev_char(struct vidd_client* client);
void vidd_find_next_word(struct vidd_client* client, char* word, intmax_t length);
void vidd_find_prev_word(struct vidd_client* client, char* word, intmax_t length);
void vidd_find_next_word_under_cursor(struct vidd_client* client);
void vidd_find_prev_word_under_cursor(struct vidd_client* client);
void vidd_goto_next_paragraph(struct vidd_client* client);
void vidd_goto_prev_paragraph(struct vidd_client* client);
void vidd_repeat_last_find(struct vidd_client* client);
void vidd_repeat_last_find_reverse(struct vidd_client* client);

void vidd_mode_swap(struct vidd_client* client, int com);
void vidd_enter_normal_mode(struct vidd_client* client);
void vidd_enter_insert_mode(struct vidd_client* client);
void vidd_exit_insert_mode(struct vidd_client* client);
void vidd_enter_insert_mode_right(struct vidd_client* client);
void vidd_enter_select_mode(struct vidd_client* client);
void vidd_enter_line_select_mode(struct vidd_client* client);
void vidd_enter_command_mode(struct vidd_client* client);
void vidd_enter_find_next_mode(struct vidd_client* client);
void vidd_enter_find_prev_mode(struct vidd_client* client);
void vidd_enter_replace_mode(struct vidd_client* client);
void vidd_enter_window_move_mode(struct vidd_client* client);

void vidd_client_up(struct vidd_client* client);
void vidd_client_down(struct vidd_client* client);
void vidd_client_prev(struct vidd_client* client);
void vidd_client_next(struct vidd_client* client);
void vidd_client_left(struct vidd_client* client);
void vidd_client_right(struct vidd_client* client);
void vidd_swap_split(struct vidd_client* client);
void vidd_swap_active(void);
void vidd_swap(struct vidd_client* client);

void vidd_open_empty(struct vidd_client* client);

void vidd_load_file_data(struct vidd_client* client);
void vidd_save_file_data(struct vidd_client* client);

void vidd_run_make(struct vidd_client* client);

void vidd_load_syntax(struct vidd_client* client, char* args);
void vidd_set(struct vidd_client* client, char* args);
void vidd_man(struct vidd_client* client, char* args);
void vidd_edit(struct vidd_client* client, char* args);
void vidd_fuzzy_find_open(struct vidd_client* client);
void vidd_run_command_in_frame(struct vidd_client* client, char* args);
void vidd_run_command(struct vidd_client* client, char* args);
void vidd_run_command_in_vsplit(struct vidd_client* client, char* args);
void vidd_run_command_in_floating_window(struct vidd_client* client, char* args);
void vidd_open_in_floating_window(struct vidd_client* client, char* args);
void vidd_split(struct vidd_client* client, char* args);
void vidd_duplicate(struct vidd_client* client);
void vidd_vsplit(struct vidd_client* client, char* args);
void vidd_write(struct vidd_client* client, char* args);
void vidd_write_all(struct vidd_client* client, char* args);
void vidd_write_quit_all(struct vidd_client* client, char* args);
void vidd_write_quit(struct vidd_client* client, char* args);
void vidd_client_quit(struct vidd_client* client, char* args);
void vidd_client_force_quit(struct vidd_client* client, char* args);
void vidd_force_exit_all(struct vidd_client* client, char* args);
void vidd_exit_all(struct vidd_client* client, char* args);

void vidd_void(struct vidd_client* client);

#endif